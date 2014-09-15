from Scripts import math
from Scripts import random

from Scripts import graphiti
from Scripts import std

import sys
import mutex
import time

LEAP_API_PATH = '../raindance/Lib/LeapSDK/lib'
sys.path.insert(0, LEAP_API_PATH)

import Leap, sys, thread, time
from Leap import CircleGesture, KeyTapGesture, ScreenTapGesture, SwipeGesture

import threading

mutex = threading.Lock()
leap_context = dict()

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

        # Get the most recent frame and report some basic information
        frame = controller.frame()
        
        state = [len(frame.hands)]
        for hand in frame.hands:
            if hand.is_left:
                state.append("Left")
            else:
                state.append("Right")

        mutex.acquire()
        try:
            leap_context["state"] = "{0} : H{1}, F{2}, T{3}, G{4}".format(frame.timestamp, len(frame.hands), len(frame.fingers), len(frame.tools), len(frame.gestures()))
        finally:
            mutex.release()

        '''
        #print >>self.log, "Frame id: %d, timestamp: %d, hands: %d, fingers: %d, tools: %d, gestures: %d" % (
        #      frame.id, frame.timestamp, len(frame.hands), len(frame.fingers), len(frame.tools), len(frame.gestures()))

        # Get hands
        for hand in frame.hands:

            handType = "Left hand" if hand.is_left else "Right hand"

            print "  %s, id %d, position: %s" % (
                handType, hand.id, hand.palm_position)

            # Get the hand's normal vector and direction
            normal = hand.palm_normal
            direction = hand.direction

            # Calculate the hand's pitch, roll, and yaw angles
            print "  pitch: %f degrees, roll: %f degrees, yaw: %f degrees" % (
                direction.pitch * Leap.RAD_TO_DEG,
                normal.roll * Leap.RAD_TO_DEG,
                direction.yaw * Leap.RAD_TO_DEG)

            # Get arm bone
            arm = hand.arm
            print "  Arm direction: %s, wrist position: %s, elbow position: %s" % (
                arm.direction,
                arm.wrist_position,
                arm.elbow_position)

            # Get fingers
            for finger in hand.fingers:

                print "    %s finger, id: %d, length: %fmm, width: %fmm" % (
                    self.finger_names[finger.type()],
                    finger.id,
                    finger.length,
                    finger.width)

                # Get bones
                for b in range(0, 4):
                    bone = finger.bone(b)
                    print "      Bone: %s, start: %s, end: %s, direction: %s" % (
                        self.bone_names[bone.type],
                        bone.prev_joint,
                        bone.next_joint,
                        bone.direction)

        # Get tools
        for tool in frame.tools:

            print "  Tool id: %d, position: %s, direction: %s" % (
                tool.id, tool.tip_position, tool.direction)

        # Get gestures
        for gesture in frame.gestures():
            if gesture.type == Leap.Gesture.TYPE_CIRCLE:
                circle = CircleGesture(gesture)

                # Determine clock direction using the angle between the pointable and the circle normal
                if circle.pointable.direction.angle_to(circle.normal) <= Leap.PI/2:
                    clockwiseness = "clockwise"
                else:
                    clockwiseness = "counterclockwise"

                # Calculate the angle swept since the last frame
                swept_angle = 0
                if circle.state != Leap.Gesture.STATE_START:
                    previous_update = CircleGesture(controller.frame(1).gesture(circle.id))
                    swept_angle =  (circle.progress - previous_update.progress) * 2 * Leap.PI

                print "  Circle id: %d, %s, progress: %f, radius: %f, angle: %f degrees, %s" % (
                        gesture.id, self.state_names[gesture.state],
                        circle.progress, circle.radius, swept_angle * Leap.RAD_TO_DEG, clockwiseness)

            if gesture.type == Leap.Gesture.TYPE_SWIPE:
                swipe = SwipeGesture(gesture)
                print "  Swipe id: %d, state: %s, position: %s, direction: %s, speed: %f" % (
                        gesture.id, self.state_names[gesture.state],
                        swipe.position, swipe.direction, swipe.speed)

            if gesture.type == Leap.Gesture.TYPE_KEY_TAP:
                keytap = KeyTapGesture(gesture)
                print "  Key Tap id: %d, %s, position: %s, direction: %s" % (
                        gesture.id, self.state_names[gesture.state],
                        keytap.position, keytap.direction )

            if gesture.type == Leap.Gesture.TYPE_SCREEN_TAP:
                screentap = ScreenTapGesture(gesture)
                print "  Screen Tap id: %d, %s, position: %s, direction: %s" % (
                        gesture.id, self.state_names[gesture.state],
                        screentap.position, screentap.direction )

        if not (frame.hands.is_empty and frame.gestures().is_empty):
            print ""
        '''

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
    mutex.acquire()
    try:
        cam_pos = graphiti.get_attribute("og:space:camera:position")
        #graphiti.set_attribute("og:space:title", "string", leap_context["state"])
        graphiti.set_attribute("og:space:title", "string", str(cam_pos))
    finally:
        mutex.release()

def start():

    leap_context["state"] = "start"
    listener = LeapListener()
    controller = Leap.Controller()
    controller.add_listener(listener)

    Scripts = [
        ["Leap Motion", [
            ["Dummy", "pass"],
        ]],
    ]

    unreg_command = ""
    for script in Scripts:
        for s in script[1]:
            unreg_command += 'graphiti.unregister_script("' + s[0] + '")\n'

    for script in Scripts:
        reg_command = ""
        for s in script[1]:
            reg_command += 'graphiti.register_script("' + s[0] + '", "' + s[1] + '")\n'

        graphiti.register_script(script[0], unreg_command + "\n" + reg_command) 

    graphiti.register_script("==========", "pass")

    graphiti.register_script("#started", "leap.on_started()")
    graphiti.register_script('#idle', 'leap.on_idle()')
    graphiti.add_job("#idle", 0.1);

    graphiti.create_window("OpenGraphiti : Data Visualization Engine", 0, 0)
    graphiti.create_entity("graph")
    graphiti.create_view("space")
    graphiti.start()
    
    controller.remove_listener(listener)
