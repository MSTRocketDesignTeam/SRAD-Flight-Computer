'''
Author: Seth Sievers
Date: 4/26/24
Desc: A module for implementing the serial communication state machine
'''
import cfg
import serial
import serial.tools.list_ports
import time 

# -------------------------------- COMMUNICATE ------------------------------- #
def connect():
        if cfg.COM_STATE == -1: #! RESET SRAD BOARD
                print('Resetting...', end='', flush=True)
                time.sleep(0.1)
                cfg.SER.baudrate = 1200 
                time.sleep(0.1)
                cfg.SER.close()
                time.sleep(12) # wait for the board to reset
                cfg.COM_STATE = 0 # go back to scan
        elif cfg.COM_STATE == 0: #! BOARD IS DISCONNECTED
                print('Scanning...', end = '', flush=True)
                while (cfg.COM_STATE == 0):
                        while (len(serial.tools.list_ports.comports()) < 1):
                                time.sleep(0.5)
                        for port in serial.tools.list_ports.comports():
                                if ('Arduino Micro' in port.description):
                                        print('Found...', end='', flush=True)
                                        cfg.COM_PORT = port.name
                                        cfg.COM_STATE = 1
                                        break 
        elif cfg.COM_STATE == 1: #! ATTEMPT CONNECTION 
                time.sleep(0.1)
                try:
                        cfg.SER = serial.Serial(cfg.COM_PORT, 115200, timeout=1, write_timeout=0.5)
                        time.sleep(0.1)
                        cfg.SER.reset_input_buffer()
                        cfg.SER.reset_output_buffer()
                        cfg.COM_STATE = 3
                except: 
                        # error connecting, go back to DC state
                        # need connection to reset so no point trying
                        cfg.COM_STATE = 0
        elif cfg.COM_STATE == 3: #! SEND 'HELLO'
                print('Hello...', end='', flush=True)
                try:
                        time.sleep(0.1)
                        cfg.SER.write(bytes([0xAA])) # send PC 'hello' packet 
                        cfg.COM_STATE = 4
                except: 
                        # error sending packet, reset the board
                        cfg.COM_STATE = -1
        elif cfg.COM_STATE == 4: #! WAIT FOR 'ACK'
                try:
                        read_char = cfg.SER.read(1) # read the ack packet
                        if ((len(read_char) == 1) and (ord(read_char) == 0xBB)):
                                # success!, go to next state
                                print('ACK...', end='', flush=True)
                                cfg.COM_STATE = 5
                        else: 
                                # something went wrong, try again 
                                cfg.COM_STATE = -1
                except: 
                        # something went wrong, reset and retry
                        cfg.COM_STATE = -1
        elif cfg.COM_STATE == 5: #! CONNECTION SUCCESSFUL, START PING TASK
                print('Connected\n', flush=True)
                while True:
                        pass

        return
# ---------------------------------------------------------------------------- #