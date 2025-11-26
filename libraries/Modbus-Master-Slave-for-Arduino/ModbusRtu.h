/**
 * @file 	ModbusRtu.h
 * @version     1.21
 * @date        2016.02.21
 * @author 	Samuel Marco i Armengol
 * @contact     sammarcoarmengol@gmail.com
 * @contribution Helium6072
 * @contribution gabrielsan
 *
 * @description
 *  Arduino library for communicating with Modbus devices
 *  over RS232/USB/485 via RTU protocol.
 *
 *  Further information:
 *  http://modbus.org/
 *  http://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf
 *
 * @license
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; version
 *  2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @defgroup setup Modbus Object Instantiation/Initialization
 * @defgroup loop Modbus Object Management
 * @defgroup buffer Modbus Buffer Management
 * @defgroup discrete Modbus Function Codes for Discrete Coils/Inputs
 * @defgroup register Modbus Function Codes for Holding/Input Registers
 *
 */

#include <inttypes.h>
#include "Arduino.h"

#include "ModbusRtu_header.h"


/**
 * @struct modbus_t
 * @brief
 * Master query structure:
 * This includes all the necessary fields to make the Master generate a Modbus query.
 * A Master may keep several of these structures and send them cyclically or
 * use them according to program needs.
 */

/**
 * @class Modbus
 * @brief
 * Arduino class library for communicating with Modbus devices over
 * USB/RS232/485 (via RTU protocol).
 */
class Modbus
{
private:
    Stream *port; //!< Pointer to Stream class object (Either HardwareSerial or SoftwareSerial)
    uint8_t u8id; //!< 0=master, 1..247=slave number
    uint8_t u8txenpin; //!< flow control pin: 0=USB or RS-232 mode, >1=RS-485 mode
    uint8_t u8state;
    uint8_t u8lastError;
    uint8_t au8Buffer[MAX_BUFFER];
    uint8_t u8BufferSize;
    uint8_t u8lastRec;
    uint16_t *au16regs;
    uint16_t u16InCnt, u16OutCnt, u16errCnt;
    uint16_t u16timeOut;
    uint32_t u32time, u32timeOut, u32overTime;
    uint8_t u8regsize;

    void sendTxBuffer();
    int8_t getRxBuffer();
    uint16_t calcCRC(uint8_t u8length);
    uint8_t validateAnswer();
    uint8_t validateRequest();
    void get_FC1();
    void get_FC3();
    int8_t process_FC1( uint16_t *regs, uint8_t u8size );
    int8_t process_FC3( uint16_t *regs, uint8_t u8size );
    int8_t process_FC5( uint16_t *regs, uint8_t u8size );
    int8_t process_FC6( uint16_t *regs, uint8_t u8size );
    int8_t process_FC15( uint16_t *regs, uint8_t u8size );
    int8_t process_FC16( uint16_t *regs, uint8_t u8size );
    void buildException( uint8_t u8exception ); // build exception message

public:
    Modbus(uint8_t u8id, Stream& port, uint8_t u8txenpin =0);

    void start();
    void setTimeOut( uint16_t u16timeOut); //!<write communication watch-dog timer
    uint16_t getTimeOut(); //!<get communication watch-dog timer value
    boolean getTimeOutState(); //!<get communication watch-dog timer state
    int8_t query( modbus_t telegram ); //!<only for master
    int8_t poll(); //!<cyclic poll for master
    int8_t poll( uint16_t *regs, uint8_t u8size ); //!<cyclic poll for slave
    uint16_t getInCnt(); //!<number of incoming messages
    uint16_t getOutCnt(); //!<number of outcoming messages
    uint16_t getErrCnt(); //!<error counter
    uint8_t getID(); //!<get slave ID between 1 and 247
    uint8_t getState();
    uint8_t getLastError(); //!<get last error message
    void setID( uint8_t u8id ); //!<write new ID for the slave
    void setTxendPinOverTime( uint32_t u32overTime );
    void end(); //!<finish any communication and release serial communication port
    void resetTimeout(); //!<>
    //
    // Deprecated functions

    // Deprecated: Use constructor: "Modbus m(0,Serial,0)" instead.
    Modbus(uint8_t u8id=0, uint8_t u8serno=0, uint8_t u8txenpin=0) __attribute__((deprecated));

    // Deprecated: Use "start()" instead.
    template<typename T_Stream>
    void begin(T_Stream* port_, long u32speed_) __attribute__((deprecated));

    // Deprecated: Use "start()" instead.
    template<typename T_Stream>
    void begin(T_Stream* port_, long u32speed_, uint8_t u8txenpin_) __attribute__((deprecated));

    // Deprecated: Use "start()" instead.
    void begin(long u32speed = 19200) __attribute__((deprecated));
};

