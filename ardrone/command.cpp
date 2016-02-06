// -------------------------------------------------------------------------
// CV Drone (= OpenCV + AR.Drone)
// Copyright(C) 2013 puku0x
// https://github.com/puku0x/cvdrone
//
// This source file is part of CV Drone library.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of EITHER:
// (1) The GNU Lesser General Public License as published by the Free
//     Software Foundation; either version 2.1 of the License, or (at
//     your option) any later version. The text of the GNU Lesser
//     General Public License is included with this library in the
//     file cvdrone-license-LGPL.txt.
// (2) The BSD-style license that is included with this library in
//     the file cvdrone-license-BSD.txt.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files
// cvdrone-license-LGPL.txt and cvdrone-license-BSD.txt for more details.
// -------------------------------------------------------------------------

#include "ardrone.h"

// --------------------------------------------------------------------------
// ARDrone::initCommand()
// Description  : Initialize AT command.
// Return value : SUCCESS: 1  FAILURE: 0
// --------------------------------------------------------------------------
int ARDrone::initCommand(void)
{
    DoLog("Initialize command");
	
	// Open the IP address and port
    if (!sockCommand.open(ip, ARDRONE_COMMAND_PORT))
	{
        CVDRONE_ERROR("UDPSocket::open(port=%d) failed. (%s, %d)\n", ARDRONE_COMMAND_PORT, __FILE__, __LINE__);
        return 0;
    }

    // AR.Drone 2.0
    if (version.major == ARDRONE_VERSION_2)
	{
        // Send undocumented command
        sockCommand.sendf("AT*PMODE=%d,%d\r", seq++, 2);
        Sleep(100);

        // Send undocumented command
        sockCommand.sendf("AT*MISC=%d,%d,%d,%d,%d\r", seq++, 2, 20, 2000, 3000);
        Sleep(100);

        // Send flat trim
        sockCommand.sendf("AT*FTRIM=%d,\r", seq++);
        Sleep(100);

        // Set the configuration IDs
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"custom:session_id\",\"%s\"\r", seq++, ARDRONE_SESSION_ID);
        Sleep(100);
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"custom:profile_id\",\"%s\"\r", seq++, ARDRONE_PROFILE_ID);
        Sleep(100);
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"custom:application_id\",\"%s\"\r", seq++, ARDRONE_APPLOCATION_ID);
        Sleep(100);

        //// Enable video
        //sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        //sockCommand.sendf("AT*CONFIG=%d,\"general:video_enable\",\"TRUE\"\r", seq++);
        //Sleep(100);

        //// Disable bitrate control mode
        //sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        //sockCommand.sendf("AT*CONFIG=%d,\"video:bitrate_ctrl_mode\",\"0\"\r", seq++);
        //Sleep(100);

        // Set video codec
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, CConfig::GetSingleton()->GetVideoCodec());
		//sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, 0x81); // H264_360P_CODEC
        //sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, 0x82); // MP4_360P_H264_720P_CODEC
        //sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, 0x83); // H264_720P_CODEC
        //sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, 0x88); // MP4_360P_H264_360P_CODEC
        Sleep(100);

        // Set video channel to default
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_channel\",\"0\"\r", seq++);
        Sleep(100);
    }
    // AR.Drone 1.0
    else
	{
        // Send undocumented command
        sockCommand.sendf("AT*PMODE=%d,%d\r", seq++, 2);
        Sleep(100);

        // Send undocumented command
        sockCommand.sendf("AT*MISC=%d,%d,%d,%d,%d\r", seq++, 2, 20, 2000, 3000);
        Sleep(100);

        // Send flat trim
        sockCommand.sendf("AT*FTRIM=%d,\r", seq++);
        Sleep(100);

        //// Enable video
        //sockCommand.sendf("AT*CONFIG=%d,\"general:video_enable\",\"TRUE\"\r", seq++);
        //Sleep(100);

        //// Disable bitrate control mode
        //sockCommand.sendf("AT*CONFIG=%d,\"video:bitrate_ctrl_mode\",\"0\"\r", seq++);
        //Sleep(100);

        // Set video codec
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, 0x20);   // UVLC_CODEC
        //sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, 0x40); // P264_CODEC
        Sleep(100);
        
        // Set video channel to default
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_channel\",\"0\"\r", seq++);
        Sleep(100);
    }	

    // Enable thread loop
    bflagCommand = true;

    // Create a thread
	UINT id;
    threadCommand = (HANDLE)_beginthreadex(NULL, 0, runCommand, this, 0, &id);
    if (threadCommand == INVALID_HANDLE_VALUE)
	{
        CVDRONE_ERROR("_beginthreadex() was failed. (%s, %d)\n", __FILE__, __LINE__);
        return 0;
    }

	DoLog("Command initialized");

    return 1;
}

// --------------------------------------------------------------------------
// ARDrone::loopCommand()
// Description  : Thread function for AT Command.
// Return value : SUCCESS:0
// --------------------------------------------------------------------------
UINT ARDrone::loopCommand(void)
{
    while (bflagCommand)
	{
        // Reset Watch-Dog every 100ms
        LockCommand();
        if(0 == sockCommand.sendf("AT*COMWDG=%d\r", seq++))
		{
			bNeedCommandRestart = true;
			break;
		}
		UnlockCommand();

		// Set the update time
		CSCommandWD.Enter();
		lCommandUpdateTime = m_pWatch->Time();
		CSCommandWD.Leave();

        Sleep(100);
    }

	bflagCommand = false;

    return 0;
}


// --------------------------------------------------------------------------
// ARDrone::takeoff()
// Description  : Take off the AR.Drone.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::takeoff(void)
{
    // Reset emergency
    resetWatchDog();
    resetEmergency();

    // Send take off
    LockCommand();
    sockCommand.sendf("AT*REF=%d,290718208\r", seq++);
    UnlockCommand();

	Sleep(100);
}

// --------------------------------------------------------------------------
// ARDrone::landing()
// Description  : Land the AR.Drone.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::landing(void)
{
    // Reset emergency
    resetWatchDog();
    resetEmergency();

    // Send landing
    LockCommand();
    sockCommand.sendf("AT*REF=%d,290717696\r", seq++);
    UnlockCommand();
}

// --------------------------------------------------------------------------
// ARDrone::emergency()
// Description  : Emergency stop.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::emergency(void)
{
    // Send emergency
    LockCommand();
    sockCommand.sendf("AT*REF=%d,290717952\r", seq++);
    UnlockCommand();
}

// --------------------------------------------------------------------------
// ARDrone::move(X velocity[m/s], Y velocity[m/s], Rotational speed[rad/s])
// Description  : Move the AR.Drone in 2D plane.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::move(double vx, double vy, double vr)
{
    move3D(vx, vy, 0.0, vr);
}

// --------------------------------------------------------------------------
// ARDrone::move3D(X velocity[m/s], Y velocity[m/s], Z velocity[m/s], Rotational speed[rad/s])
// Description  : Move the AR.Drone in 3D space.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::move3D(double vx, double vy, double vz, double vr)
{
    // AR.Drone is flying
    if (!onGround())
	{
        const float gain = 0.4f;
        float v[4] = {-vy*gain, -vx*gain, vz*gain, -vr*gain};
        int mode = (fabs(vx) > 0.0 || fabs(vy) > 0.0);

        // Send a command
        LockCommand();
        sockCommand.sendf("AT*PCMD=%d,%d,%d,%d,%d,%d\r", seq++, mode, *(int*)(&v[0]), *(int*)(&v[1]), *(int*)(&v[2]), *(int*)(&v[3]));
        UnlockCommand();
    }
}

// --------------------------------------------------------------------------
// ARDrone::setCamera(Camera channel)
// Description  : Change the camera channel.
//                AR.Drone1.0 supports 0, 1, 2, 3.
//                AR.Drone2.0 supports 0, 1.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::setCamera(int channel)
{
    // Enable mutex lock
   LockCommand();

    // AR.Drone 2.0
    if (version.major == ARDRONE_VERSION_2)
	{
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_channel\",\"%d\"\r", seq++, channel % 2);
    }
    // AR.Drone 1.0
    else
	{
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_channel\",\"%d\"\r", seq++, channel % 4);
    }

    // Disable mutex lock
    UnlockCommand();

    Sleep(100);
}

// --------------------------------------------------------------------------
// ARDrone::setAnimation(Flight animation ID, Duration[s])
// Description  : Run specified flight animation.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::setAnimation(int id, int duration)
{
    // Send animation command
    LockCommand();
    sockCommand.sendf("AT*ANIM=%d,%d,%d\r", seq++, id, duration);
    //sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
    //sockCommand.sendf("AT*CONFIG=%d,\"leds:flight_anim\",\"%d,%d\"\r", seq++, id, duration);
    UnlockCommand();
    //Sleep(100);
}

// --------------------------------------------------------------------------
// ARDrone::setLED(LED animation ID, Frequency[Hz], Duration[s])
// Description  : Run specified LED animation.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::setLED(int id, float freq, int duration)
{
    // Send a command
    LockCommand();
    sockCommand.sendf("AT*LED=%d,%d,%d,%d\r", seq++, id, *(int*)(&freq), duration);
    //sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
    //sockCommand.sendf("AT*CONFIG=%d,\"leds:leds_anim\",\"%d,%d,%d\"\r", seq++, id, *(int*)(&freq), duration);
    UnlockCommand();

    //Sleep(100);
}

// --------------------------------------------------------------------------
// ARDrone::startVideoRecord()
// Start recording video.
// This function is only for AR.Drone 2.0
// You should set a USB key with > 100MB to your drone
// Return value NONE
// --------------------------------------------------------------------------
void ARDrone::startVideoRecord(void)
{
    if (version.major == ARDRONE_VERSION_2)
	{
        // Finalize video
        finalizeVideo();

        // Enable video record
        LockCommand();
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_on_usb\",\"TRUE\"\r", seq++);
        UnlockCommand();
        Sleep(100);

        // Output video with MP4_360P_H264_720P_CODEC
        LockCommand();
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, 0x82);
        UnlockCommand();
        Sleep(100);

        // Initialize video
		if(0 == initVideo())
		{
			DoLog("Failed to restart video after start record, watchdog should catch this", MSG_ERROR);
			bNeedVideoRestart = true;
		}
    }
}

// --------------------------------------------------------------------------
// ARDrone::stopVideoRecord()
// Stop recording video.
// This function is only for AR.Drone 2.0
// Return value NONE
// --------------------------------------------------------------------------
void ARDrone::stopVideoRecord(void)
{
    if (version.major == ARDRONE_VERSION_2)
	{
        // Finalize video
        finalizeVideo();

        // Disable video record
        LockCommand();
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_on_usb\",\"FALSE\"\r", seq++);
        UnlockCommand();
        Sleep(100);

        // Output video with 360P
        LockCommand();
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        //sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, 0x81);
		sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, CConfig::GetSingleton()->GetVideoCodec());
        UnlockCommand();
        Sleep(100);

        // Initialize video
		if(0 == initVideo())
		{
			DoLog("Failed to restart video after stop record, watchdog should catch this", MSG_ERROR);
			bNeedVideoRestart = true;
		}
    }
}

// --------------------------------------------------------------------------
// ARDrone::resetWatchDog()
// Description  : Stop hovering.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::resetWatchDog(void)
{
	LockNavdata();
	bool bDoReset = ( (navdata.ardrone_state & ARDRONE_COM_WATCHDOG_MASK) == ARDRONE_COM_WATCHDOG_MASK);
	UnlockNavdata();

    // If AR.Drone is in Watch-Dog, reset it
    if(bDoReset)
	{
        LockCommand();
        sockCommand.sendf("AT*COMWDG=%d\r", seq++);
        UnlockCommand();
    }
}

// --------------------------------------------------------------------------
// ARDrone::resetEmergency()
// Description  : Disable the emergency lock.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::resetEmergency(void)
{
	LockNavdata();
	bool bDoReset = ( (navdata.ardrone_state & ARDRONE_EMERGENCY_MASK) == ARDRONE_EMERGENCY_MASK);
	UnlockNavdata();

    // If AR.Drone is in emergency, reset it
    if(bDoReset)
	{
        LockCommand();
        sockCommand.sendf("AT*REF=%d,290717952\r", seq++);
        UnlockCommand();
    }
}

// --------------------------------------------------------------------------
// ARDrone::finalizeCommand()
// Description  : Finalize AT command
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::finalizeCommand(void)
{
    // Disable thread loop
    bflagCommand = false;

	DoLog("Finalize command");

    // Destroy the thread
    if (threadCommand != INVALID_HANDLE_VALUE)
	{
        //WaitForSingleObject(threadCommand, INFINITE);
		DWORD dwRes = WaitForSingleObject(threadCommand, 2500);
		switch(dwRes)
		{
			case WAIT_TIMEOUT:
				DoLog("Wait for command: thread timed out !", MSG_ERROR);
				TerminateThread(threadCommand, -1);
				break;

			case WAIT_FAILED:
				DoLog("Wait for command: thread failed !", MSG_ERROR);
				TerminateThread(threadCommand, -1);
				break;

			default:
				break;
		}

        if(FALSE == CloseHandle(threadCommand))
		{
			DoLog("Failed to close command thread handle", MSG_ERROR);
		}
        threadCommand = INVALID_HANDLE_VALUE;
    }

    // Close the socket
    sockCommand.close();

	DoLog("Command finalized");
}