import serial
import serial.tools.list_ports
import csv 

if __name__ == '__main__':
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
        
        # decode the packets and store data into nested lists
        print('Decoding Packets')
        accel_data = []
        pressure_data = []
        for i in range(len(data_list)):
                # read the packet header byte 
                byte_val = data_list.pop(0)
                if ('50' in byte_val):
                        # acceleration packet 
                        time = int('0x' + data_list[3],16)*(16**3) + int('0x' + data_list[2],16)*(16**2) + int('0x' + data_list[1], 16)*16 + int('0x' + data_list[0],16)

                        # remove data from the list 
                        data_list = data_list[10::]
                        print(time / 1000)
                        #print(data_list[0:20])

                elif ('46' in byte_val):
                        # pressure packet
                        data_list = data_list[8::]
                        pass
                else:
                        # end of data
                        print('end of data') 
                        print(data_list[0:20])
                        break 
        

        # release the serial port 
        ser.close() 

