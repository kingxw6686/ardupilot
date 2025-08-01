#include "AP_Vehicle_config.h"

#if AP_VEHICLE_ENABLED

#include "AP_Vehicle.h"
#include <AP_InertialSensor/AP_InertialSensor_rate_config.h>

#include <AP_BLHeli/AP_BLHeli.h>
#include <AP_Common/AP_FWVersion.h>
#include <AP_Arming/AP_Arming.h>
#include <AP_Frsky_Telem/AP_Frsky_Parameters.h>
#include <AP_Logger/AP_Logger.h>
#include <AP_Mission/AP_Mission.h>
#include <AP_OSD/AP_OSD.h>
#include <SRV_Channel/SRV_Channel.h>
#include <AP_Motors/AP_Motors.h>
#include <AR_Motors/AP_MotorsUGV.h>
#include <AP_CheckFirmware/AP_CheckFirmware.h>
#include <GCS_MAVLink/GCS.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_CHIBIOS
#include <AP_HAL_ChibiOS/sdcard.h>
#include <AP_HAL_ChibiOS/hwdef/common/stm32_util.h>
#endif
#include <AP_DDS/AP_DDS_Client.h>
#if HAL_WITH_IO_MCU
#include <AP_IOMCU/AP_IOMCU.h>
extern AP_IOMCU iomcu;
#endif
#include <AP_Scripting/AP_Scripting.h>

#define SCHED_TASK(func, rate_hz, max_time_micros, prio) SCHED_TASK_CLASS(AP_Vehicle, &vehicle, func, rate_hz, max_time_micros, prio)

/*
  2nd group of parameters
 */
const AP_Param::GroupInfo AP_Vehicle::var_info[] = {

    // 1: RunCam

#if HAL_GYROFFT_ENABLED
    // @Group: FFT_
    // @Path: ../AP_GyroFFT/AP_GyroFFT.cpp
    AP_SUBGROUPINFO(gyro_fft, "FFT_",  2, AP_Vehicle, AP_GyroFFT),
#endif

#if HAL_VISUALODOM_ENABLED
    // @Group: VISO
    // @Path: ../AP_VisualOdom/AP_VisualOdom.cpp
    AP_SUBGROUPINFO(visual_odom, "VISO",  3, AP_Vehicle, AP_VisualOdom),
#endif

#if AP_VIDEOTX_ENABLED
    // @Group: VTX_
    // @Path: ../AP_VideoTX/AP_VideoTX.cpp
    AP_SUBGROUPINFO(vtx, "VTX_",  4, AP_Vehicle, AP_VideoTX),
#endif

#if HAL_MSP_ENABLED
    // @Group: MSP
    // @Path: ../AP_MSP/AP_MSP.cpp
    AP_SUBGROUPINFO(msp, "MSP",  5, AP_Vehicle, AP_MSP),
#endif

#if HAL_WITH_FRSKY_TELEM_BIDIRECTIONAL
    // @Group: FRSKY_
    // @Path: ../AP_Frsky_Telem/AP_Frsky_Parameters.cpp
    AP_SUBGROUPINFO(frsky_parameters, "FRSKY_", 6, AP_Vehicle, AP_Frsky_Parameters),
#endif

#if HAL_GENERATOR_ENABLED
    // @Group: GEN_
    // @Path: ../AP_Generator/AP_Generator.cpp
    AP_SUBGROUPINFO(generator, "GEN_", 7, AP_Vehicle, AP_Generator),
#endif

#if AP_EXTERNAL_AHRS_ENABLED
    // @Group: EAHRS
    // @Path: ../AP_ExternalAHRS/AP_ExternalAHRS.cpp
    AP_SUBGROUPINFO(externalAHRS, "EAHRS", 8, AP_Vehicle, AP_ExternalAHRS),
#endif

#if HAL_EFI_ENABLED
    // @Group: EFI
    // @Path: ../AP_EFI/AP_EFI.cpp
    AP_SUBGROUPINFO(efi, "EFI", 9, AP_Vehicle, AP_EFI),
#endif

#if AP_AIRSPEED_ENABLED
    // @Group: ARSPD
    // @Path: ../AP_Airspeed/AP_Airspeed.cpp
    AP_SUBGROUPINFO(airspeed, "ARSPD", 10, AP_Vehicle, AP_Airspeed),
#endif

#if AP_CUSTOMROTATIONS_ENABLED
    // @Group: CUST_ROT
    // @Path: ../AP_CustomRotations/AP_CustomRotations.cpp
    AP_SUBGROUPINFO(custom_rotations, "CUST_ROT", 11, AP_Vehicle, AP_CustomRotations),
#endif

#if HAL_WITH_ESC_TELEM
    // @Group: ESC_TLM
    // @Path: ../AP_ESC_Telem/AP_ESC_Telem.cpp
    AP_SUBGROUPINFO(esc_telem, "ESC_TLM", 12, AP_Vehicle, AP_ESC_Telem),
#endif

#if AP_AIS_ENABLED
    // @Group: AIS_
    // @Path: ../AP_AIS/AP_AIS.cpp
    AP_SUBGROUPINFO(ais, "AIS_",  13, AP_Vehicle, AP_AIS),
#endif

#if AP_FENCE_ENABLED
    // @Group: FENCE_
    // @Path: ../AC_Fence/AC_Fence.cpp
    AP_SUBGROUPINFO(fence, "FENCE_", 14, AP_Vehicle, AC_Fence),
#endif

#if AP_OPENDRONEID_ENABLED
    // @Group: DID_
    // @Path: ../AP_OpenDroneID/AP_OpenDroneID.cpp
    AP_SUBGROUPINFO(opendroneid, "DID_", 15, AP_Vehicle, AP_OpenDroneID),
#endif

#if AP_TEMPERATURE_SENSOR_ENABLED
    // @Group: TEMP
    // @Path: ../AP_TemperatureSensor/AP_TemperatureSensor.cpp
    AP_SUBGROUPINFO(temperature_sensor, "TEMP", 16, AP_Vehicle, AP_TemperatureSensor),
#endif

#if HAL_NMEA_OUTPUT_ENABLED
    // @Group: NMEA_
    // @Path: ../AP_NMEA_Output/AP_NMEA_Output.cpp
    AP_SUBGROUPINFO(nmea, "NMEA_", 17, AP_Vehicle, AP_NMEA_Output),
#endif

#if AP_DDS_ENABLED
    // @Group: DDS
    // @Path: ../AP_DDS/AP_DDS_Client.cpp
    AP_SUBGROUPPTR(dds_client, "DDS", 18, AP_Vehicle, AP_DDS_Client),
#endif

#if AP_KDECAN_ENABLED
    // @Group: KDE_
    // @Path: ../AP_KDECAN/AP_KDECAN.cpp
    AP_SUBGROUPINFO(kdecan, "KDE_",  19, AP_Vehicle, AP_KDECAN),
#endif

#if APM_BUILD_COPTER_OR_HELI || APM_BUILD_TYPE(APM_BUILD_ArduPlane) || APM_BUILD_TYPE(APM_BUILD_Rover)
    // @Param: FLTMODE_GCSBLOCK
    // @DisplayName: Flight mode block from GCS
    // @Description: Bitmask of flight modes to disable for GCS selection. Mode can still be accessed via RC or failsafe.
    // @Bitmask{Copter}: 0:Stabilize
    // @Bitmask{Copter}: 1:Acro
    // @Bitmask{Copter}: 2:AltHold
    // @Bitmask{Copter}: 3:Auto
    // @Bitmask{Copter}: 4:Guided
    // @Bitmask{Copter}: 5:Loiter
    // @Bitmask{Copter}: 6:Circle
    // @Bitmask{Copter}: 7:Drift
    // @Bitmask{Copter}: 8:Sport
    // @Bitmask{Copter}: 9:Flip
    // @Bitmask{Copter}: 10:AutoTune
    // @Bitmask{Copter}: 11:PosHold
    // @Bitmask{Copter}: 12:Brake
    // @Bitmask{Copter}: 13:Throw
    // @Bitmask{Copter}: 14:Avoid_ADSB
    // @Bitmask{Copter}: 15:Guided_NoGPS
    // @Bitmask{Copter}: 16:Smart_RTL
    // @Bitmask{Copter}: 17:FlowHold
    // @Bitmask{Copter}: 18:Follow
    // @Bitmask{Copter}: 19:ZigZag
    // @Bitmask{Copter}: 20:SystemID
    // @Bitmask{Copter}: 21:Heli_Autorotate
    // @Bitmask{Copter}: 22:Auto RTL
    // @Bitmask{Copter}: 23:Turtle
    // @Bitmask{Plane}: 0:Manual
    // @Bitmask{Plane}: 1:Circle
    // @Bitmask{Plane}: 2:Stabilize
    // @Bitmask{Plane}: 3:Training
    // @Bitmask{Plane}: 4:ACRO
    // @Bitmask{Plane}: 5:FBWA
    // @Bitmask{Plane}: 6:FBWB
    // @Bitmask{Plane}: 7:CRUISE
    // @Bitmask{Plane}: 8:AUTOTUNE
    // @Bitmask{Plane}: 9:Auto
    // @Bitmask{Plane}: 10:Loiter
    // @Bitmask{Plane}: 11:Takeoff
    // @Bitmask{Plane}: 12:AVOID_ADSB
    // @Bitmask{Plane}: 13:Guided
    // @Bitmask{Plane}: 14:THERMAL
    // @Bitmask{Plane}: 15:QSTABILIZE
    // @Bitmask{Plane}: 16:QHOVER
    // @Bitmask{Plane}: 17:QLOITER
    // @Bitmask{Plane}: 18:QACRO
    // @Bitmask{Plane}: 19:QAUTOTUNE
    // @Bitmask{Rover}: 0:Manual
    // @Bitmask{Rover}: 1:Acro
    // @Bitmask{Rover}: 2:Steering
    // @Bitmask{Rover}: 3:Loiter
    // @Bitmask{Rover}: 4:Follow
    // @Bitmask{Rover}: 5:Simple
    // @Bitmask{Rover}: 6:Circle
    // @Bitmask{Rover}: 7:Auto
    // @Bitmask{Rover}: 8:RTL
    // @Bitmask{Rover}: 9:SmartRTL
    // @Bitmask{Rover}: 10:Guided
    // @Bitmask{Rover}: 11:Dock
    // @User: Standard
    AP_GROUPINFO("FLTMODE_GCSBLOCK", 20, AP_Vehicle, flight_mode_GCS_block, 0),
#endif // APM_BUILD_COPTER_OR_HELI || APM_BUILD_TYPE(APM_BUILD_ArduPlane) || APM_BUILD_TYPE(APM_BUILD_Rover)


#if AP_NETWORKING_ENABLED
    // @Group: NET_
    // @Path: ../AP_Networking/AP_Networking.cpp
    AP_SUBGROUPINFO(networking, "NET_", 21, AP_Vehicle, AP_Networking),

    /*
      the NET_Pn_ parameters need to be in AP_Vehicle as otherwise we
      are too deep in the parameter tree
     */

#if AP_NETWORKING_REGISTER_PORT_ENABLED
#if AP_NETWORKING_NUM_PORTS > 0
    // @Group: NET_P1_
    // @Path: ../AP_Networking/AP_Networking_port.cpp
    AP_SUBGROUPINFO(networking.ports[0], "NET_P1_", 22, AP_Vehicle, AP_Networking::Port),
#endif

#if AP_NETWORKING_NUM_PORTS > 1
    // @Group: NET_P2_
    // @Path: ../AP_Networking/AP_Networking_port.cpp
    AP_SUBGROUPINFO(networking.ports[1], "NET_P2_", 23, AP_Vehicle, AP_Networking::Port),
#endif

#if AP_NETWORKING_NUM_PORTS > 2
    // @Group: NET_P3_
    // @Path: ../AP_Networking/AP_Networking_port.cpp
    AP_SUBGROUPINFO(networking.ports[2], "NET_P3_", 24, AP_Vehicle, AP_Networking::Port),
#endif

#if AP_NETWORKING_NUM_PORTS > 3
    // @Group: NET_P4_
    // @Path: ../AP_Networking/AP_Networking_port.cpp
    AP_SUBGROUPINFO(networking.ports[3], "NET_P4_", 25, AP_Vehicle, AP_Networking::Port),
#endif
#endif  // AP_NETWORKING_REGISTER_PORT_ENABLED
#endif // AP_NETWORKING_ENABLED

#if AP_FILTER_ENABLED
    // @Group: FILT
    // @Path: ../Filter/AP_Filter.cpp
    AP_SUBGROUPINFO(filters, "FILT", 26, AP_Vehicle, AP_Filters),
#endif

#if AP_STATS_ENABLED
    // @Group: STAT
    // @Path: ../AP_Stats/AP_Stats.cpp
    AP_SUBGROUPINFO(stats, "STAT", 27, AP_Vehicle, AP_Stats),
#endif

#if AP_SCRIPTING_ENABLED
    // @Group: SCR_
    // @Path: ../AP_Scripting/AP_Scripting.cpp
    AP_SUBGROUPINFO(scripting, "SCR_", 28, AP_Vehicle, AP_Scripting),
#endif

#if HAL_LOGGING_ENABLED
    // @Group: LOG
    // @Path: ../AP_Logger/AP_Logger.cpp
    AP_SUBGROUPINFO(logger, "LOG",  29, AP_Vehicle, AP_Logger),
#endif

#if AP_GRIPPER_ENABLED
    // @Group: GRIP_
    // @Path: ../AP_Gripper/AP_Gripper.cpp
    AP_SUBGROUPINFO(gripper, "GRIP_", 30, AP_Vehicle, AP_Gripper),
#endif

#if AP_SERIALMANAGER_ENABLED
    // @Group: SERIAL
    // @Path: ../AP_SerialManager/AP_SerialManager.cpp
    AP_SUBGROUPINFO(serial_manager, "SERIAL", 31, AP_Vehicle, AP_SerialManager),
#endif

#if AP_RPM_ENABLED
    // @Group: RPM
    // @Path: ../AP_RPM/AP_RPM.cpp
    AP_SUBGROUPINFO(rpm_sensor, "RPM", 32, AP_Vehicle, AP_RPM),
#endif

    AP_GROUPEND
};

// reference to the vehicle. using AP::vehicle() here does not work on clang
#if APM_BUILD_TYPE(APM_BUILD_UNKNOWN) || APM_BUILD_TYPE(APM_BUILD_AP_Periph)
AP_Vehicle& vehicle = *AP_Vehicle::get_singleton();
#else
extern AP_Vehicle& vehicle;
#endif

/*
  setup is called when the sketch starts
 */
void AP_Vehicle::setup()
{
    // load the default values of variables listed in var_info[]
    AP_Param::setup_sketch_defaults();

#if AP_SERIALMANAGER_ENABLED
    // initialise serial port
    serial_manager.init_console();
#endif

    DEV_PRINTF("\n\nInit %s"
                        "\n\nFree RAM: %u\n",
                        AP::fwversion().fw_string,
                        (unsigned)hal.util->available_memory());

#if AP_CHECK_FIRMWARE_ENABLED
    check_firmware_print();
#endif

    // validate the static parameter table, then load persistent
    // values from storage:
    AP_Param::check_var_info();
    load_parameters();

#if CONFIG_HAL_BOARD == HAL_BOARD_CHIBIOS
    if (AP_BoardConfig::get_sdcard_slowdown() != 0) {
        // user wants the SDcard slower, we need to remount
        sdcard_stop();
        sdcard_retry();
    }
#endif

#if AP_SCHEDULER_ENABLED
    // initialise the main loop scheduler
    const AP_Scheduler::Task *tasks;
    uint8_t task_count;
    uint32_t log_bit;
    get_scheduler_tasks(tasks, task_count, log_bit);
    AP::scheduler().init(tasks, task_count, log_bit);

    // time per loop - this gets updated in the main loop() based on
    // actual loop rate
    G_Dt = scheduler.get_loop_period_s();
#endif

    // this is here for Plane; its failsafe_check method requires the
    // RC channels to be set as early as possible for maximum
    // survivability.
    set_control_channels();

#if HAL_GCS_ENABLED
    // initialise serial manager as early as sensible to get
    // diagnostic output during boot process.  We have to initialise
    // the GCS singleton first as it sets the global mavlink system ID
    // which may get used very early on.
    gcs().init();
#endif

#if AP_SERIALMANAGER_ENABLED
#if HAL_WITH_IO_MCU
    if (BoardConfig.io_enabled()) {
        serial_manager.set_protocol_and_baud(HAL_UART_IOMCU_IDX, AP_SerialManager::SerialProtocol_IOMCU, 0);
    }
#endif
    // initialise serial ports
    serial_manager.init();
#endif
#if HAL_GCS_ENABLED
    gcs().setup_console();
#endif

#if AP_SCRIPTING_ENABLED
#if AP_SCRIPTING_SERIALDEVICE_ENABLED
    // must be done now so ports are registered and drivers get set up properly
    // (in particular mavlink which checks during init_ardupilot())
    scripting.init_serialdevice_ports();
#endif
#endif

#if AP_NETWORKING_ENABLED
    networking.init();
#endif

#if AP_SCHEDULER_ENABLED
    // Register scheduler_delay_cb, which will run anytime you have
    // more than 5ms remaining in your call to hal.scheduler->delay
    hal.scheduler->register_delay_callback(scheduler_delay_callback, 5);
#endif

#if HAL_MSP_ENABLED
    // call MSP init before init_ardupilot to allow for MSP sensors
    msp.init();
#endif

#if AP_EXTERNAL_AHRS_ENABLED
    // call externalAHRS init before init_ardupilot to allow for external sensors
    externalAHRS.init();
#endif

#if HAL_GENERATOR_ENABLED
    generator.init();
#endif

#if AP_STATS_ENABLED
    // initialise stats module
    stats.init();
#endif

    BoardConfig.init();

#if HAL_CANMANAGER_ENABLED
    can_mgr.init();
#endif

#if HAL_LOGGING_ENABLED
    logger.init(get_log_bitmask(), get_log_structures(), get_num_log_structures());
#endif

    // init cargo gripper
#if AP_GRIPPER_ENABLED
    AP::gripper().init();
#endif

    // init_ardupilot is where the vehicle does most of its initialisation.
    init_ardupilot();

#if AP_SCRIPTING_ENABLED
    scripting.init();
#endif // AP_SCRIPTING_ENABLED

#if AP_AIRSPEED_ENABLED
    airspeed.init();
    if (airspeed.enabled()) {
        airspeed.calibrate(true);
    } 
#if APM_BUILD_TYPE(APM_BUILD_ArduPlane)
    else {
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "No airspeed sensor");
    }
#endif
#endif  // AP_AIRSPEED_ENABLED


#if AP_SRV_CHANNELS_ENABLED
    AP::srv().init();
#endif

    // gyro FFT needs to be initialized really late
#if HAL_GYROFFT_ENABLED
#if AP_SCHEDULER_ENABLED
    gyro_fft.init(AP::scheduler().get_loop_rate_hz());
#else
    gyro_fft.init(1000);
#endif
#endif
#if HAL_HOTT_TELEM_ENABLED
    hott_telem.init();
#endif
#if HAL_VISUALODOM_ENABLED
    // init library used for visual position estimation
    visual_odom.init();
#endif

#if AP_VIDEOTX_ENABLED
    vtx.init();
#endif

#if AP_SMARTAUDIO_ENABLED
    smartaudio.init();
#endif

#if AP_TRAMP_ENABLED
    tramp.init();
#endif

#if AP_PARAM_KEY_DUMP
    AP_Param::show_all(hal.console, true);
#endif

    send_watchdog_reset_statustext();

#if AP_OPENDRONEID_ENABLED
    opendroneid.init();
#endif

// init EFI monitoring
#if HAL_EFI_ENABLED
    efi.init();
#endif

#if AP_TEMPERATURE_SENSOR_ENABLED
    temperature_sensor.init();
#endif

#if AP_KDECAN_ENABLED
    kdecan.init();
#endif

#if AP_AIS_ENABLED
    ais.init();
#endif

#if HAL_NMEA_OUTPUT_ENABLED
    nmea.init();
#endif

#if AP_FENCE_ENABLED
    fence.init();
    fence_init();
#endif

#if AP_CUSTOMROTATIONS_ENABLED
    custom_rotations.init();
#endif

#if AP_FILTER_ENABLED
    filters.init();
#endif

#if HAL_WITH_ESC_TELEM && HAL_GYROFFT_ENABLED
    for (uint8_t i = 0; i<ESC_TELEM_MAX_ESCS; i++) {
        esc_noise[i].set_cutoff_frequency(2);
    }
#endif

#if AP_RPM_ENABLED
    rpm_sensor.init();
#endif

    // invalidate count in case an enable parameter changed during
    // initialisation
    AP_Param::invalidate_count();

    GCS_SEND_TEXT(MAV_SEVERITY_INFO, "ArduPilot Ready");

#if AP_DDS_ENABLED
    if (!init_dds_client()) {
        GCS_SEND_TEXT(MAV_SEVERITY_ERROR, "%s Failed to Initialize", AP_DDS_Client::msg_prefix);
    }
#endif

#if AP_IBUS_TELEM_ENABLED
    ibus_telem.init();
#endif
}

void AP_Vehicle::loop()
{
#if AP_SCHEDULER_ENABLED
    scheduler.loop();
    G_Dt = scheduler.get_loop_period_s();
#else
    hal.scheduler->delay(1);
    G_Dt = 0.001;
#endif

    if (!done_safety_init) {
        /*
          disable safety if requested. This is delayed till after the
          first loop has run to ensure that all servos have received
          an update for their initial values. Otherwise we may end up
          briefly driving a servo to a position out of the configured
          range which could damage hardware
        */
        done_safety_init = true;
        BoardConfig.init_safety();

        // send RC output mode info if available
        char banner_msg[50];
        if (hal.rcout->get_output_mode_banner(banner_msg, sizeof(banner_msg))) {
            GCS_SEND_TEXT(MAV_SEVERITY_INFO, "%s", banner_msg);
        }
    }
    const uint32_t new_internal_errors = AP::internalerror().errors();
    if(_last_internal_errors != new_internal_errors) {
        LOGGER_WRITE_ERROR(LogErrorSubsystem::INTERNAL_ERROR, LogErrorCode::INTERNAL_ERRORS_DETECTED);
        GCS_SEND_TEXT(MAV_SEVERITY_CRITICAL, "Internal Errors 0x%x", (unsigned)new_internal_errors);
        _last_internal_errors = new_internal_errors;
    }
}

#if AP_SCHEDULER_ENABLED
/*
  scheduler table - all regular tasks apart from the fast_loop()
  should be listed here.

  All entries in this table must be ordered by priority.

  This table is interleaved with the table presnet in each of the
  vehicles to determine the order in which tasks are run.  Convenience
  methods SCHED_TASK and SCHED_TASK_CLASS are provided to build
  entries in this structure:

SCHED_TASK arguments:
 - name of static function to call
 - rate (in Hertz) at which the function should be called
 - expected time (in MicroSeconds) that the function should take to run
 - priority (0 through 255, lower number meaning higher priority)

SCHED_TASK_CLASS arguments:
 - class name of method to be called
 - instance on which to call the method
 - method to call on that instance
 - rate (in Hertz) at which the method should be called
 - expected time (in MicroSeconds) that the method should take to run
 - priority (0 through 255, lower number meaning higher priority)

 */
const AP_Scheduler::Task AP_Vehicle::scheduler_tasks[] = {
#if HAL_GYROFFT_ENABLED
    FAST_TASK_CLASS(AP_GyroFFT,    &vehicle.gyro_fft,       sample_gyros),
#endif
#if AP_AIRSPEED_ENABLED
    SCHED_TASK_CLASS(AP_Airspeed,  &vehicle.airspeed,       update,                   10, 100, 41),    // NOTE: the priority number here should be right before Plane's calc_airspeed_errors
#endif
#if COMPASS_CAL_ENABLED
    SCHED_TASK_CLASS(Compass,      &vehicle.compass,        cal_update,     100, 200, 75),
#endif
    SCHED_TASK_CLASS(AP_Notify,    &vehicle.notify,         update,                   50, 300, 78),
#if HAL_NMEA_OUTPUT_ENABLED
    SCHED_TASK_CLASS(AP_NMEA_Output, &vehicle.nmea,         update,                   50, 50, 180),
#endif
#if HAL_GYROFFT_ENABLED
    SCHED_TASK_CLASS(AP_GyroFFT,   &vehicle.gyro_fft,       update,                  400, 50, 205),
    SCHED_TASK_CLASS(AP_GyroFFT,   &vehicle.gyro_fft,       update_parameters,         1, 50, 210),
#endif
#if AP_INERTIALSENSOR_HARMONICNOTCH_ENABLED && !AP_INERTIALSENSOR_FAST_SAMPLE_WINDOW_ENABLED
    SCHED_TASK(update_dynamic_notch_at_specified_rate,      LOOP_RATE,                    200, 215),
#endif
#if AP_VIDEOTX_ENABLED
    SCHED_TASK_CLASS(AP_VideoTX,   &vehicle.vtx,            update,                    2, 100, 220),
#endif
#if AP_TRAMP_ENABLED
    SCHED_TASK_CLASS(AP_Tramp,     &vehicle.tramp,          update,                   50,  50, 225),
#endif
    SCHED_TASK(send_watchdog_reset_statustext,         0.1,     20, 225),
#if HAL_WITH_ESC_TELEM
    // This update function is responsible for checking timeouts and invalidating the ESC telemetry data.
    // Be mindful of this if you are planning to reduce the frequency from 100Hz.
    SCHED_TASK_CLASS(AP_ESC_Telem, &vehicle.esc_telem,      update,                  100,  50, 230),
#endif
#if AP_SERVO_TELEM_ENABLED
    SCHED_TASK_CLASS(AP_Servo_Telem, &vehicle.servo_telem,  update,                   50,  50, 231),
#endif
#if HAL_GENERATOR_ENABLED
    SCHED_TASK_CLASS(AP_Generator, &vehicle.generator,      update,                   10,  50, 235),
#endif
#if AP_OPENDRONEID_ENABLED
    SCHED_TASK_CLASS(AP_OpenDroneID, &vehicle.opendroneid,  update,                   10,  50, 236),
#endif
#if AP_NETWORKING_ENABLED
    SCHED_TASK_CLASS(AP_Networking, &vehicle.networking,    update,                   10,  50, 238),
#endif
#if AP_RPM_ENABLED
    SCHED_TASK_CLASS(AP_RPM, &vehicle.rpm_sensor, update,                             50, 100, 239),
#endif
#if OSD_ENABLED
    SCHED_TASK(publish_osd_info, 1, 10, 240),
#endif
#if AP_TEMPERATURE_SENSOR_ENABLED
    SCHED_TASK_CLASS(AP_TemperatureSensor, &vehicle.temperature_sensor, update,        5, 50, 242),
#endif
#if HAL_INS_ACCELCAL_ENABLED
    SCHED_TASK(accel_cal_update,                                                      10, 100, 245),
#endif
#if AP_FENCE_ENABLED
    SCHED_TASK_CLASS(AC_Fence,     &vehicle.fence,          update,                   10, 100, 248),
#endif
#if AP_AIS_ENABLED
    SCHED_TASK_CLASS(AP_AIS,       &vehicle.ais,            update,                    5, 100, 249),
#endif
#if HAL_EFI_ENABLED
    SCHED_TASK_CLASS(AP_EFI,       &vehicle.efi,            update,                   50, 200, 250),
#endif
#if AP_GRIPPER_ENABLED
    SCHED_TASK_CLASS(AP_Gripper,   &vehicle.gripper,        update,                   10,  75, 251),
#endif
    SCHED_TASK(one_Hz_update,                                                         1, 100, 252),
#if HAL_WITH_ESC_TELEM && HAL_GYROFFT_ENABLED
    SCHED_TASK(check_motor_noise,      5,     50, 252),
#endif
#if AP_FILTER_ENABLED
    SCHED_TASK_CLASS(AP_Filters,   &vehicle.filters,        update,                   1, 100, 252),
#endif
#if AP_STATS_ENABLED
    SCHED_TASK_CLASS(AP_Stats,             &vehicle.stats,            update,           1, 100, 252),
#endif
#if AP_ARMING_ENABLED
    SCHED_TASK(update_arming,          1,     50, 253),
#endif
};

void AP_Vehicle::get_common_scheduler_tasks(const AP_Scheduler::Task*& tasks, uint8_t& num_tasks)
{
    tasks = scheduler_tasks;
    num_tasks = ARRAY_SIZE(scheduler_tasks);
}

/*
 *  a delay() callback that processes MAVLink packets. We set this as the
 *  callback in long running library initialisation routines to allow
 *  MAVLink to process packets while waiting for the initialisation to
 *  complete
 */
void AP_Vehicle::scheduler_delay_callback()
{
#if APM_BUILD_TYPE(APM_BUILD_Replay)
    // compass.init() delays, so we end up here.
    return;
#endif

    static uint32_t last_1hz, last_50hz, last_5s;

#if HAL_LOGGING_ENABLED
    AP_Logger &logger = AP::logger();

    // don't allow potentially expensive logging calls:
    logger.EnableWrites(false);
#endif

    const uint32_t tnow = AP_HAL::millis();
    if (tnow - last_1hz > 1000) {
        last_1hz = tnow;
        GCS_SEND_MESSAGE(MSG_HEARTBEAT);
        GCS_SEND_MESSAGE(MSG_SYS_STATUS);
    }
    if (tnow - last_50hz > 20) {
        last_50hz = tnow;
#if HAL_GCS_ENABLED
        gcs().update_receive();
        gcs().update_send();
#endif
        _singleton->notify.update();
    }
    if (tnow - last_5s > 5000) {
        last_5s = tnow;
        if (AP_BoardConfig::in_config_error()) {
            GCS_SEND_TEXT(MAV_SEVERITY_CRITICAL, "Config Error: fix problem then reboot");
        } else {
            GCS_SEND_TEXT(MAV_SEVERITY_INFO, "Initialising ArduPilot");
        }
    }

#if HAL_LOGGING_ENABLED
    logger.EnableWrites(true);
#endif
}
#endif  // AP_SCHEDULER_ENABLED

// if there's been a watchdog reset, notify the world via a statustext:
void AP_Vehicle::send_watchdog_reset_statustext()
{
    if (!hal.util->was_watchdog_reset()) {
        return;
    }
    const AP_HAL::Util::PersistentData &pd = hal.util->last_persistent_data;
    (void)pd;  // in case !HAL_GCS_ENABLED
    GCS_SEND_TEXT(MAV_SEVERITY_CRITICAL,
                    "WDG: T%d SL%u FL%u FT%u FA%x FTP%u FLR%x FICSR%u MM%u MC%u IE%u IEC%u TN:%.4s",
                    pd.scheduler_task,
                    pd.semaphore_line,
                    pd.fault_line,
                    pd.fault_type,
                    (unsigned)pd.fault_addr,
                    pd.fault_thd_prio,
                    (unsigned)pd.fault_lr,
                    (unsigned)pd.fault_icsr,
                    pd.last_mavlink_msgid,
                    pd.last_mavlink_cmd,
                    (unsigned)pd.internal_errors,
                    (unsigned)pd.internal_error_count,
                    pd.thread_name4
        );
}

bool AP_Vehicle::is_crashed() const
{
#if AP_ARMING_ENABLED
    if (AP::arming().is_armed()) {
        return false;
    }
    return AP::arming().last_disarm_method() == AP_Arming::Method::CRASH;
#else
    return false;
#endif
}

#if AP_INERTIALSENSOR_HARMONICNOTCH_ENABLED
// update the harmonic notch filter for throttle based notch
void AP_Vehicle::update_throttle_notch(AP_InertialSensor::HarmonicNotch &notch)
{
#if APM_BUILD_TYPE(APM_BUILD_ArduPlane) || APM_BUILD_COPTER_OR_HELI || APM_BUILD_TYPE(APM_BUILD_Rover)
    const float ref_freq = notch.params.center_freq_hz();
    const float ref = notch.params.reference();

#if APM_BUILD_TYPE(APM_BUILD_ArduPlane) || APM_BUILD_COPTER_OR_HELI
    const AP_Motors* motors = AP::motors();
    if (motors == nullptr) {
         notch.update_freq_hz(0);
         return;
    }
    const float motors_throttle = MAX(0,motors->get_throttle_out());
    // set the harmonic notch filter frequency scaled on measured frequency
    if (notch.params.hasOption(HarmonicNotchFilterParams::Options::DynamicHarmonic)) {
        float notches[INS_MAX_NOTCHES];
        uint8_t motor_num = 0;
        for (uint8_t i = 0; i < AP_MOTORS_MAX_NUM_MOTORS; i++) {
            float motor_throttle = 0;
            if (motors->get_thrust(i, motor_throttle)) {
                notches[motor_num] = ref_freq * sqrtf(MAX(0, motor_throttle) / ref);
                motor_num++;
            }
            if (motor_num >= INS_MAX_NOTCHES) {
                break;
            }
        }
        notch.update_frequencies_hz(motor_num, notches);
    } else
#else  // APM_BUILD_Rover
    const AP_MotorsUGV *motors = AP::motors_ugv();
    const float motors_throttle = motors != nullptr ? abs(motors->get_throttle() * 0.01f) : 0;
#endif
    {
        float throttle_freq = ref_freq * sqrtf(MAX(0,motors_throttle) / ref);

        notch.update_freq_hz(throttle_freq);
    }

#endif
}

// update the harmonic notch filter center frequency dynamically
void AP_Vehicle::update_dynamic_notch(AP_InertialSensor::HarmonicNotch &notch)
{
#if APM_BUILD_TYPE(APM_BUILD_ArduPlane)||APM_BUILD_COPTER_OR_HELI||APM_BUILD_TYPE(APM_BUILD_Rover)
    if (!notch.params.enabled()) {
        return;
    }
    const float ref_freq = notch.params.center_freq_hz();
    const float ref = notch.params.reference();
    if (is_zero(ref)) {
        notch.update_freq_hz(ref_freq);
        return;
    }

    switch (notch.params.tracking_mode()) {
        case HarmonicNotchDynamicMode::UpdateThrottle: // throttle based tracking
            // set the harmonic notch filter frequency approximately scaled on motor rpm implied by throttle
            update_throttle_notch(notch);
            break;

#if AP_RPM_ENABLED
        case HarmonicNotchDynamicMode::UpdateRPM: // rpm sensor based tracking
        case HarmonicNotchDynamicMode::UpdateRPM2: {
            uint8_t sensor = (notch.params.tracking_mode()==HarmonicNotchDynamicMode::UpdateRPM?0:1);
            float rpm;
            if (rpm_sensor.get_rpm(sensor, rpm)) {
                // set the harmonic notch filter frequency from the main rotor rpm
                notch.update_freq_hz(rpm * ref * (1.0/60));
            } else {
                notch.update_freq_hz(0);
            }
            break;
        }
#endif  // AP_RPM_ENABLED
#if HAL_WITH_ESC_TELEM
        case HarmonicNotchDynamicMode::UpdateBLHeli: // BLHeli based tracking
            // set the harmonic notch filter frequency scaled on measured frequency
            if (notch.params.hasOption(HarmonicNotchFilterParams::Options::DynamicHarmonic)) {
                float notches[INS_MAX_NOTCHES];
                // ESC telemetry will return 0 for missing data, but only after 1s
                const uint8_t num_notches = AP::esc_telem().get_motor_frequencies_hz(INS_MAX_NOTCHES, notches);
                if (num_notches > 0) {
                    notch.update_frequencies_hz(num_notches, notches);
                } else {    // throttle fallback
                    update_throttle_notch(notch);
                }
            } else {
                notch.update_freq_hz(AP::esc_telem().get_average_motor_frequency_hz() * ref);
            }
            break;
#endif
#if HAL_GYROFFT_ENABLED
        case HarmonicNotchDynamicMode::UpdateGyroFFT: // FFT based tracking
            // set the harmonic notch filter frequency scaled on measured frequency
            if (notch.params.hasOption(HarmonicNotchFilterParams::Options::DynamicHarmonic)) {
                float notches[INS_MAX_NOTCHES];
                const uint8_t peaks = gyro_fft.get_weighted_noise_center_frequencies_hz(notch.num_dynamic_notches, notches);

                if (peaks > 0) {
                    notch.set_inactive(false);
                    notch.update_frequencies_hz(peaks, notches);
                } else {    // since FFT can be used post-filter it is better to disable the notch when there is no data
                    notch.set_inactive(true);
                }
            } else {
                float center_freq = gyro_fft.get_weighted_noise_center_freq_hz();
                notch.update_freq_hz(center_freq);
            }
            break;
#endif
        case HarmonicNotchDynamicMode::Fixed: // static
        default:
            notch.update_freq_hz(ref_freq);
            break;
    }
#endif // APM_BUILD_TYPE(APM_BUILD_ArduPlane)||APM_BUILD_COPTER_OR_HELI||APM_BUILD_TYPE(APM_BUILD_Rover)
}

// run notch update at either loop rate or 200Hz
void AP_Vehicle::update_dynamic_notch_at_specified_rate()
{
    for (auto &notch : ins.harmonic_notches) {
        if (notch.params.hasOption(HarmonicNotchFilterParams::Options::LoopRateUpdate)) {
            update_dynamic_notch(notch);
        } else {
            // decimated update at 200Hz
            const uint32_t now = AP_HAL::millis();
            const uint8_t i = &notch - &ins.harmonic_notches[0];
            if (now - _last_notch_update_ms[i] > 5) {
                _last_notch_update_ms[i] = now;
                update_dynamic_notch(notch);
            }
        }
    }
}
#endif  // AP_INERTIALSENSOR_HARMONICNOTCH_ENABLED

void AP_Vehicle::notify_no_such_mode(uint8_t mode_number)
{
    GCS_SEND_TEXT(MAV_SEVERITY_WARNING,"No such mode %u", mode_number);
    LOGGER_WRITE_ERROR(LogErrorSubsystem::FLIGHT_MODE, LogErrorCode(mode_number));
}

// reboot the vehicle in an orderly manner, doing various cleanups and
// flashing LEDs as appropriate
void AP_Vehicle::reboot(bool hold_in_bootloader)
{
    if (should_zero_rc_outputs_on_reboot()) {
        SRV_Channels::zero_rc_outputs();
    }

    // Notify might want to blink some LEDs:
    AP_Notify::flags.firmware_update = 1;
    notify.update();

    // force safety on
    hal.rcout->force_safety_on();

    // flush pending parameter writes
    AP_Param::flush();

    // do not process incoming mavlink messages while we delay:
    hal.scheduler->register_delay_callback(nullptr, 5);

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    // need to ensure the ack goes out:
    hal.serial(0)->flush();
#endif

    // delay to give the ACK a chance to get out, the LEDs to flash,
    // the IO board safety to be forced on, the parameters to flush, ...
    hal.scheduler->delay(200);

#if HAL_WITH_IO_MCU
    iomcu.soft_reboot();
#endif

    hal.scheduler->reboot(hold_in_bootloader);
}

#if OSD_ENABLED
void AP_Vehicle::publish_osd_info()
{
#if AP_MISSION_ENABLED
    AP_Mission *mission = AP::mission();
    if (mission == nullptr) {
        return;
    }
    AP_OSD *osd = AP::osd();
    if (osd == nullptr) {
        return;
    }
    AP_OSD::NavInfo nav_info;
    if(!get_wp_distance_m(nav_info.wp_distance)) {
        return;
    }
    float wp_bearing_deg;
    if (!get_wp_bearing_deg(wp_bearing_deg)) {
        return;
    }
    nav_info.wp_bearing = (int32_t)wp_bearing_deg * 100; // OSD expects cd
    if (!get_wp_crosstrack_error_m(nav_info.wp_xtrack_error)) {
        return;
    }
    nav_info.wp_number = mission->get_current_nav_index();
    osd->set_nav_info(nav_info);
#endif
}
#endif

void AP_Vehicle::get_osd_roll_pitch_rad(float &roll, float &pitch) const
{
#if AP_AHRS_ENABLED
    roll = ahrs.get_roll_rad();
    pitch = ahrs.get_pitch_rad();
#else
    roll = 0.0;
    pitch = 0.0;
#endif
}

#if HAL_INS_ACCELCAL_ENABLED

#ifndef HAL_CAL_ALWAYS_REBOOT
// allow for forced reboot after accelcal
#define HAL_CAL_ALWAYS_REBOOT 0
#endif

/*
  update accel cal
 */
void AP_Vehicle::accel_cal_update()
{
    if (hal.util->get_soft_armed()) {
        return;
    }
    ins.acal_update();

#if AP_AHRS_ENABLED
    // check if new trim values, and set them
    Vector3f trim_rad;
    if (ins.get_new_trim(trim_rad)) {
        ahrs.set_trim(trim_rad);
    }
#endif

#if HAL_CAL_ALWAYS_REBOOT
    if (ins.accel_cal_requires_reboot() &&
        !hal.util->get_soft_armed()) {
        hal.scheduler->delay(1000);
        hal.scheduler->reboot();
    }
#endif
}
#endif // HAL_INS_ACCELCAL_ENABLED

#if AP_ARMING_ENABLED
// call the arming library's update function
void AP_Vehicle::update_arming()
{
    AP::arming().update();
}
#endif

/*
  one Hz checks common to all vehicles
 */
void AP_Vehicle::one_Hz_update(void)
{
    one_Hz_counter++;

    /*
      every 10s check if using a 2M firmware on a 1M board
     */
    if (one_Hz_counter % 10U == 0) {
#if defined(BOARD_CHECK_F427_USE_1M) && (HAL_PROGRAM_SIZE_LIMIT_KB>1024)
        if (!hal.util->get_soft_armed() && check_limit_flash_1M()) {
            GCS_SEND_TEXT(MAV_SEVERITY_CRITICAL, BOARD_CHECK_F427_USE_1M);
        }
#endif
    }

    /*
      every 30s check if using a 1M firmware on a 2M board
     */
    if (one_Hz_counter % 30U == 0) {
#if defined(BOARD_CHECK_F427_USE_1M) && (HAL_PROGRAM_SIZE_LIMIT_KB<=1024)
        if (!hal.util->get_soft_armed() && !check_limit_flash_1M()) {
            GCS_SEND_TEXT(MAV_SEVERITY_INFO, BOARD_CHECK_F427_USE_2M);
        }
#endif
    }

#if AP_SCRIPTING_ENABLED
    scripting.update();
#endif

#if HAL_LOGGING_ENABLED && HAL_UART_STATS_ENABLED
    // Log data rates of physical and virtual serial ports
    hal.util->uart_log();
#if AP_SERIALMANAGER_REGISTER_ENABLED
    serial_manager.registered_ports_log();
#endif
#endif

}

void AP_Vehicle::check_motor_noise()
{
#if HAL_GYROFFT_ENABLED && HAL_WITH_ESC_TELEM
    if (!hal.util->get_soft_armed() || !gyro_fft.check_esc_noise() || !gyro_fft.using_post_filter_samples()) {
        return;
    }
#if AP_INERTIALSENSOR_HARMONICNOTCH_ENABLED
    if (ins.has_fft_notch()) {
        return;
    }
#endif

    float esc_data[ESC_TELEM_MAX_ESCS];
    const uint8_t numf = AP::esc_telem().get_motor_frequencies_hz(ESC_TELEM_MAX_ESCS, esc_data);
    bool output_error = false;

    for (uint8_t i = 0; i<numf; i++) {
        if (is_zero(esc_data[i])) {
            continue;
        }
        float energy = gyro_fft.has_noise_at_frequency_hz(esc_data[i]);
        energy = esc_noise[i].apply(energy, 0.2f);
        if (energy > 40.0f && AP_HAL::millis() - last_motor_noise_ms > 5000) {
            GCS_SEND_TEXT(MAV_SEVERITY_WARNING, "Noise %.fdB on motor %u at %.fHz", energy, i+1, esc_data[i]);
            output_error = true;
        }
    }

    if (output_error) {
        last_motor_noise_ms = AP_HAL::millis();
    }
#endif
}

#if AP_DDS_ENABLED
bool AP_Vehicle::init_dds_client()
{
    dds_client = NEW_NOTHROW AP_DDS_Client();
    if (dds_client == nullptr) {
        return false;
    }
    return dds_client->start();
}
#endif // AP_DDS_ENABLED

// Check if this mode can be entered from the GCS
#if APM_BUILD_COPTER_OR_HELI || APM_BUILD_TYPE(APM_BUILD_ArduPlane) || APM_BUILD_TYPE(APM_BUILD_Rover)
bool AP_Vehicle::block_GCS_mode_change(uint8_t mode_num, const uint8_t *mode_list, uint8_t mode_list_length) const
{
    if (mode_list == nullptr) {
        return false;
    }

    for (uint8_t i = 0; i < mode_list_length; i++) {
        // Find index of mode
        if (mode_list[i] == mode_num) {
            const uint32_t mask = 1U << i;
            return (uint32_t(flight_mode_GCS_block) & mask) != 0;
        }
    }

    return false;
}
#endif

#if AP_FENCE_ENABLED
void AP_Vehicle::fence_init()
{
    hal.scheduler->register_io_process(FUNCTOR_BIND_MEMBER(&AP_Vehicle::fence_checks_async, void));
}
#endif  // AP_FENCE_ENABLED

AP_Vehicle *AP_Vehicle::_singleton = nullptr;

AP_Vehicle *AP_Vehicle::get_singleton()
{
    return _singleton;
}

namespace AP {

AP_Vehicle *vehicle()
{
    return AP_Vehicle::get_singleton();
}

};

#endif  // AP_VEHICLE_ENABLED
