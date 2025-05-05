# Note: This is a bit rudimentary but should work
# Can be improved greatly with gui in future 

import serial
import serial.tools.list_ports
import csv 

if __name__ == '__main__':
        try: 
                # scan for the proper com port
                # named 'Arduino Micro' 
                iterator = serial.tools.list_ports.comports()
                port = 'invalid'
                for x in iterator:
                        if 'Arduino Micro' in x.description:
                                port = x
                                print(f'Selected {x.device}, {x.description}')
                                break
                assert port != 'invalid', "Error: SRAD ALtimeter Not Found"
                
                # attempt connecting to serial port 
                ser = None
                try:
                        ser = serial.Serial(x.device, 9600, timeout=1)
                except Exception as e:
                        print(f'{e}')

                # send an S to trigger status print 
                ser.write('S\n'.encode('ASCII'))
                ser.readline().decode('ASCII').strip()
                has_data = False
                for x in range(10):
                        read_line = ser.readline().decode('ASCII').strip()
                        if 'CONTAINS FLIGHT' in read_line:
                                has_data = True
                        print(read_line)
                assert has_data, "SRAD Altimeter Has No Data!"

                # send an R to trigger data dump 
                raw_output_file_path = input('Enter the path for the Raw Output File: ')
                ser.write('R\n'.encode('ASCII'))
                raw_output_file = open(raw_output_file_path, 'w')
                print('Reading from altimeter!')
                read_line = ser.readline().decode('ASCII').strip()
                while '----' not in read_line:
                        # save the header 
                        read_line = ser.readline().decode('ASCII').strip()
                        raw_output_file.write(read_line + '\n')
                
                # read the data of value into a list and save to file 
                data_list = []
                num_zeros = 0 
                while (num_zeros < 24):
                        read_line = ser.readline().decode('ASCII').strip()
                        raw_output_file.write(read_line + '\n')
                        data_list.append(read_line)
                        if '0' == read_line: 
                                num_zeros += 1
                        else: 
                                num_zeros = 0 

                # finish reading until entire FRAM has been printed 
                read_line = ser.readline().decode('ASCII').strip()
                while ('Done.' not in read_line):
                        read_line = ser.readline().decode('ASCII').strip()
                raw_output_file.close() 

                # open output csv
                csv_output_file_path = input('Enter the path for the decoded CSV output file: ')
                csv_output_pressure_file = open(csv_output_file_path+'Pressure', mode='a', newline='')
                csvwriter_pressure = csv.writer(csv_output_pressure_file)
                csvwriter_pressure.writerow(['Time (s)', 'Pressure (MilliBar)'])
                csv_output_accel_file = open(csv_output_file_path+'Accel', mode='a', newline='')
                csvwriter_accel = csv.writer(csv_output_accel_file)
                csvwriter_accel.writerow(['Time (s)', 'AccelX (g)', 'AccelY (g)', 'AccelZ (g)'])
                
                # decode the packets and store data into nested lists
                print('Decoding Packets')
                accel_data = []
                pressure_data = []
                start_time_ms = -1
                for i in range(len(data_list)):
                        # read the packet header byte 
                        byte_val = data_list.pop(0)
                        if ('50' in byte_val):
                                # acceleration packet 
                                time = int('0x' + data_list[3],16)*(16**6) + int('0x' + data_list[2],16)*(16**4) + int('0x' + data_list[1], 16)*(16**2) + int('0x' + data_list[0],16)

                                if start_time_ms == -1:
                                        start_time_ms = time

                                time -= start_time_ms

                                # decode acceleration data
                                accel_x_int = int('0x' + data_list[5], 16)*(16**2) + int('0x' + data_list[4],16)
                                accel_y_int = int('0x' + data_list[7], 16)*(16**2) + int('0x' + data_list[6],16)
                                accel_z_int = int('0x' + data_list[9], 16)*(16**2) + int('0x' + data_list[8],16)

                                # handle negatives since stored value is 2s
                                # If MSB is set, the value is (val -= (1 << n))
                                def twos_adjust(val, msB):
                                        if (len(msB) < 2):
                                                # if only 1 character then MSB is 0
                                                return val 
                                        if (int(msB[0], 16) > 7):
                                                # if val of upper nibble of msB is > 7, then msb is set
                                                return (val - (1 << 16))
                                        else: 
                                                # msb not set 
                                                return val 
                        
                                accel_x_int = twos_adjust(accel_x_int, data_list[5])
                                accel_y_int = twos_adjust(accel_y_int, data_list[7])
                                accel_z_int = twos_adjust(accel_z_int, data_list[9])

                                accel_x_g = float(accel_x_int) * float(1.0 / 512.0)
                                accel_y_g = float(accel_y_int) * float(1.0 / 512.0)
                                accel_z_g = float(accel_z_int) * float(1.0 / 512.0)

                                csvwriter_accel.writerow([f'{time/1000.0:.4f}', f'{accel_x_g:.4f}', f'{accel_y_g:.4f}', f'{accel_z_g:.4f}'])
                                print(f'AccelPkt, T:{time/1000.0:.0f}, X:{accel_x_g:.2f}g, Y:{accel_y_g:.2f}g, Z:{accel_z_g:.2f}g')

                                # accelPkt has 10 bytes of data 
                                data_list = data_list[10::] # remove data from the list 

                        elif ('46' in byte_val):
                                # pressure packet 
                                time = int('0x' + data_list[3],16)*(16**6) + int('0x' + data_list[2],16)*(16**4) + int('0x' + data_list[1], 16)*(16**2) + int('0x' + data_list[0],16)

                                if start_time_ms == -1:
                                        start_time_ms = time

                                time -= start_time_ms
                                
                                # decode the pressure value from its FP representation 
                                pressure_int = int('0x' + data_list[7],16)*(16**6) + int('0x' + data_list[6],16)*(16**4) + int('0x' + data_list[5], 16)*(16**2) + int('0x' + data_list[4],16)
                                pressureMB = float(pressure_int) / (float(1 << 16))

                                csvwriter_pressure.writerow([f'{time/1000.0:.4f}', f'{pressureMB:.4f}'])
                                print(f'PressurePkt, T:{time/1000:.0f}, P:{pressureMB:.2f} millibar')

                                # pressure packet has 8 bytes of data
                                data_list = data_list[8::] # remove data from the list 
                        
                        elif ('4A' in byte_val):
                                # event packet 
                                time = int('0x' + data_list[3],16)*(16**6) + int('0x' + data_list[2],16)*(16**4) + int('0x' + data_list[1], 16)*(16**2) + int('0x' + data_list[0],16)

                                if start_time_ms == -1:
                                        start_time_ms = time
                                time -= start_time_ms

                                event_str = ''
                                event_id = (int('0x' + data_list[4], 16))
                                if (event_id == 0):
                                        # boost start
                                        event_str = 'BOOST STARTED'
                                elif (event_id == 1):
                                        # apogee start
                                        event_str = 'APOGEE STARTED'
                                elif (event_id == 2):
                                        # fire pyro 
                                        event_str = 'PYRO FIRED'
                                elif (event_id == 3):
                                        # land start 
                                        event_str = 'APOGEE STARTED'
                                print(f'Event: "{event_str}" occured at t={time/1000.0:.1f}' )

                                # event packet has 5 bytes of data
                                data_list = data_list[5::] # remove data from the list 
                        else:
                                # end of data
                                print('end of data') 
                                print(data_list[0:20])
                                break 

                # close the csv files 
                csv_output_pressure_file.close()
                csv_output_accel_file.close()
                
                # release the serial port 
                ser.close() 
        except Exception as e:
                print(f'{e}')
                input("(Press Enter To Exit)")

