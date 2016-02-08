CXX           = g++
CXXFLAGS      = -w -O2 -Wall -D__STDC_CONSTANT_MACROS `wx-config --cxxflags --libs`
LIBS          = -lm                     \
                -lpthread               \
                -lavutil                \
                -lavformat              \
                -lavcodec               \
                -lswscale               \
                -lopencv_calib3d        \
                -lopencv_core           \
                -lopencv_features2d     \
                -lopencv_flann          \
                -lopencv_highgui        \
                -lopencv_imgcodecs      \
                -lopencv_imgproc        \
                -lopencv_ml             \
                -lopencv_objdetect      \
                -lopencv_photo          \
                -lopencv_stitching      \
                -lopencv_superres       \
                -lopencv_video          \
                -lopencv_videoio        \
                -lopencv_videostab
OBJS          = ardrone/ardrone.o \
                ardrone/command.o \
                ardrone/config.o  \
                ardrone/udp.o     \
                ardrone/tcp.o     \
                ardrone/navdata.o \
                ardrone/version.o \
                ardrone/video.o \
                AboutDialog.o \
                AppConfig.o \
                AutoPilot.o \
                ConfigDialog.o \
                CustomDrone.o \
                DroneController.o \
                Input.o \
                InputDirection.o \
                Joystick.o \
                JoystickDialog.o \
                KeyboardDialog.o \
                Utils.o
PROGRAM       = droneController.run

$(PROGRAM):     $(OBJS)
		$(CXX) $(OBJS) -o $(PROGRAM) $(CXXFLAGS) $(LDFLAGS) $(LIBS)

clean:;         rm -f *.o *~ $(PROGRAM) $(OBJS)

install:        $(PROGRAM)
		install -s $(PROGRAM) $(DEST)
