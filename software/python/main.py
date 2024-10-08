'''
Author: Seth Sievers
Date: 4/21/24
Desc: This python program is maent to be a basic interface program for the SRAD
        altimeter board
'''

import os
import threading
import serial
import serial.tools.list_ports # used to identify com number
import time 
import cfg
import communication as com 

# ----------------------------------- MAIN ----------------------------------- #
def main():
        print('------SRAD-ALTIMETER------\n')
        while (True):
                com.connect()
        # ----------------------- CONNECT TO ALTIMETER ----------------------- #
        '''
        if (not(cfg.IS_CONNECTED)):
                print('Scanning for Device...', end = '')
        while (not(cfg.IS_CONNECTED)):
                # Wait for there to be a COM port
                while (len(serial.tools.list_ports.comports()) < 1):
                        time.sleep(0.5)
                
                # Iterate through COM port list checking name 
                for port in serial.tools.list_ports.comports():
                        if ('Arduino Micro' in port.description):
                                cfg.COM_PORT = port.name
                                try: 
                                        for i in range(2): # attempt twice, reset if first fails
                                                # COM port identified, attempt connection 
                                                time.sleep(0.25)
                                                print('opening port')
                                                cfg.SER = serial.Serial(cfg.COM_PORT, 115200, timeout=1, write_timeout=0.5)
                                                print('port opened')
                                                cfg.SER.reset_output_buffer()
                                                cfg.SER.reset_input_buffer()
                                                print('sending packet')
                                                cfg.SER.write(bytes([0xAA])) # send PC 'hello' packet 
                                                print('reading packet')
                                                read_char = cfg.SER.read(1) # check for SRAD 'ACK' packet
                                                
                                                if ((len(read_char) == 1) and (ord(read_char) == 0xBB)): 
                                                        print('Connected')
                                                        cfg.IS_CONNECTED = True
                                                        print('starting thread')
                                                        cfg.PING_THREAD.start()
                                                        break
                                                else:
                                                        print('running else')
                                                        if (i == 1):
                                                                break
                                                        # reset the altimeter by changing baud rate to 1200bps and closing port
                                                        # then must wait for bootloader to time out
                                                        print('Resetting...', end='')
                                                        time.sleep(0.1)
                                                        cfg.SER.baudrate = 1200
                                                        time.sleep(0.1)
                                                        cfg.SER.close()
                                                        time.sleep(10)
                                except: 
                                        cfg.COM_PORT = None
                                        print('except running')
                                        continue 
                                break
                        
                # If nothing is found, try again 
                if (cfg.COM_PORT == None):
                        time.sleep(0.25)
                        continue                 
        # ------------------------------------------------------------ #
        '''


        
        time.sleep(10)

        cfg.ARE_THREADS_ALIVE = False

        cfg.PING_THREAD.join(timeout = 3)


        if (cfg.SER != None): cfg.SER.close() 
        pass
# ---------------------------------------------------------------------------- #

##########################
if __name__ == '__main__':
        main()
        os._exit(0) #make sure all threads exit 
##########################