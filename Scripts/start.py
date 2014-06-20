import sys
sys.path.insert(0, "./")
import os

from Scripts import *

def usage():
    exclude = ['__init__.py', 'start.py', 'standard.py']
    print("")
    print("Usage: " + sys.argv[0] + " <view>")
    print("   Available views : {}".format(
        " ".join([file.split('.')[0] for file in os.listdir('Scripts')
            if file.split('.')[-1] == "py"
            and file not in exclude])))
    print("")

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        usage()
    else:
        module = "Scripts." + sys.argv[1]
        if module in sys.modules:
            sys.modules[module].start()
        else:
            print("No module named {}".format("Scripts." + sys.argv[1]))
            usage()
