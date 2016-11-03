from Scripts import math
from Scripts import random

from Scripts import graphiti
from Scripts import std

import sys
import mutex
import time

import pprint as pp

LEAP_API_PATH = '../raindance/Lib/LeapSDK/lib'
sys.path.insert(0, LEAP_API_PATH)

import Leap, sys, thread, time
from Leap import CircleGesture, KeyTapGesture, ScreenTapGesture, SwipeGesture

import threading

mutex = threading.Lock()
leap_context = dict()
og_context = dict()

delta = 0.3

radius = 100
radius_speed = 0.0
alpha = 0.0
alpha_speed = 0.0
beta = 0.0
beta_speed = 0.0

listener = None
controller = None

class LeapListener(Leap.Listener):

    def on_init(self, controller):
        print("Leap Motion initialized.\n")

    def on_connect(self, controller):
        print("Leap Motion connected.\n")

        # Enable gestures
        controller.enable_gesture(Leap.Gesture.TYPE_CIRCLE);
        controller.enable_gesture(Leap.Gesture.TYPE_KEY_TAP);
        controller.enable_gesture(Leap.Gesture.TYPE_SCREEN_TAP);
        controller.enable_gesture(Leap.Gesture.TYPE_SWIPE);

    def on_disconnect(self, controller):
        # Note: not dispatched when running in a debugger.
        print("Leap Motion disconnected.")

    def on_exit(self, controller):
        print("Leap Motion exited.")

    def on_frame(self, controller):

        count = 50

        position = Leap.Vector()
        direction = Leap.Vector()
        velocity = Leap.Vector()

        for i in range(0, count - 1):
            hand = controller.frame(i).hands
            if hand[0].is_valid:
                position = position + hand[0].palm_position
                direction = direction + hand[0].direction
                velocity = velocity + hand[0].palm_velocity

        confidence = controller.frame().hands[0].confidence
        grab = controller.frame().hands[0].grab_strength

        position = position / count
        direction = direction / count
        velocity = velocity / count

        mutex.acquire()
        try:
            leap_context["velocity"] = velocity
            leap_context["position"] = position
            leap_context["direction"] = direction
            leap_context["box"] = controller.frame().interaction_box
            leap_context["confidence"] = confidence
            leap_context["grab"] = grab
        finally:
            mutex.release()


    def state_string(self, state):
        if state == Leap.Gesture.STATE_START:
            return "STATE_START"

        if state == Leap.Gesture.STATE_UPDATE:
            return "STATE_UPDATE"

        if state == Leap.Gesture.STATE_STOP:
            return "STATE_STOP"

        if state == Leap.Gesture.STATE_INVALID:
            return "STATE_INVALID"

def on_started():
    if len(sys.argv) == 3:
        if sys.argv[2].endswith(".json"):
            print("Loading ")
            std.load_json(sys.argv[2]) 
        else:
            print("Unrecognized format <'" + sys.argv[2] + "'> !")

def on_idle():

    global radius
    global radius_speed

    global alpha
    global alpha_speed

    global beta
    global beta_speed

    hand_position = None
    hand_direction = None
    hand_velocity = None
    box = None
    confidence = None
    grab = None

    mutex.acquire()
    try:
        if "position" in leap_context:
            hand_position = leap_context["position"]
        if "direction" in leap_context:
            hand_direction = leap_context["direction"]
        if "velocity" in leap_context:
            hand_velocity = leap_context["velocity"]
        if "box" in leap_context:
            box = leap_context["box"]
        if "confidence" in leap_context:
            confidence = leap_context["confidence"]
        if "grab" in leap_context:
            grab = leap_context["grab"]
    finally:
        mutex.release()

    camera_position = graphiti.get_attribute("og:space:camera:position")

    cam_pos = Leap.Vector()
    cam_pos.x = camera_position[0]
    cam_pos.y = camera_position[1]
    cam_pos.z = camera_position[2]

    alpha_acceleration = 0.0
    beta_acceleration = 0.0
    radius_acceleration = 0.0
    action = ""
    x_action = ""
    y_action = ""
    z_action = ""
    if not (hand_position is None or hand_direction is None or hand_velocity is None or box is None or confidence < 0.1):

        hand_position = box.normalize_point(hand_position)
        hand_velocity = box.normalize_point(hand_velocity)

        hand_pos = Leap.Vector()
        hand_pos.x = 2.0 * hand_position.z - 1.0
        hand_pos.y = -(2.0 * hand_position.y - 1.0)
        hand_pos.z = 2.0 * hand_position.x - 1.0

        if hand_pos.z > 0.0:
            z_action = "Right "
            z_sign = 1.0
        elif hand_pos.z < -0.0:
            z_action = "Left "
            z_sign = -1.0
        else:
            z_sign = 0.0

        if hand_pos.y > 0.0:
            y_action = "Down "
            y_sign = 1.0
        elif hand_pos.y < -0.0:
            y_sign = -1.0
            y_action = "Up "
        else:
            y_sign = 0.0

        if hand_pos.x > 0.0:
            x_action = " Back"
            x_sign = 1.0
        elif hand_pos.x < -0.0:
            x_action = " Front"
            x_sign = -1.0
        else:
            x_sign = 0.0

        threshold = 0.45
        if abs(hand_pos.z) >= threshold:
            action += z_action
            hand_pos.z = (1.0 / threshold) * (hand_pos.z - z_sign * threshold)
            alpha_acceleration = z_sign * (abs(hand_pos.z * hand_pos.z)) / 300

        threshold = 0.3
        if abs(hand_pos.y) >= threshold:
            action += y_action
            hand_pos.y = (1.0 / threshold) * (hand_pos.y - y_sign * threshold)
            beta_acceleration = -y_sign * 0.1

        if abs(hand_pos.x) >= threshold:
            action += x_action
            hand_pos.x = (1.0 / threshold) * (hand_pos.x - x_sign * threshold)
            radius_acceleration = x_sign *  0.1

        if len(action) == 0:
            action += "Neutral"

    if grab == 1.0:
        alpha_speed = 0.0
        beta_speed = 0.0
        radius_speed = 0.0
        alpha_acceleration = 0.0
        beta_acceleration = 0.0
        radius_acceleration = 0.0
        graphiti.set_attribute("og:space:title", "string", "Freeze")
        return

    alpha_speed += alpha_acceleration
    alpha += alpha_speed

    beta_speed += beta_acceleration
    beta += beta_speed 

    radius_speed += radius_acceleration
    radius += radius_speed

    max_beta = 100.0
    min_beta = - 100.0
    if beta > max_beta:
        beta = max_beta
    if beta < min_beta:
        beta = min_beta

    max_radius = 200
    min_radius = 10
    if radius > max_radius:
        radius = max_radius
    if radius < min_radius:
        radius = min_radius

    cam_pos.x = radius * math.cos(alpha)
    cam_pos.y = beta
    cam_pos.z = radius * math.sin(alpha)

    graphiti.set_attribute("og:space:camera:position", "vec3", "{0} {1} {2}".format(cam_pos.x, cam_pos.y, cam_pos.z))
    graphiti.set_attribute("og:space:title", "string", action)
    graphiti.set_attribute("og:space:camera:target", "vec3", "0.0 {0} 0.0".format(cam_pos.y))

    alpha_speed = alpha_speed * 0.8
    beta_speed = beta_speed * 0.8
    radius_speed = radius_speed * 0.8

def start():

    leap_context["state"] = "start"
    listener = LeapListener()
    controller = Leap.Controller()
    controller.add_listener(listener)

    graphiti.create_window("OpenGraphiti : Data Visualization Engine", 1024, 728)
    graphiti.create_entity("graph")
    graphiti.create_visualizer("space")
    graphiti.create_visualizer("logo")
    
    graphiti.register_script("#started", "leap.on_started()")
    graphiti.register_script('#idle', 'leap.on_idle()')
    graphiti.add_job("#idle", delta);

    graphiti.start()
    
    controller.remove_listener(listener)
