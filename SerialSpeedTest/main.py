import serial
import time
import csv

serial_com_port = 'COM4'
baud_rate = 460800

# ----------------------------------- MAIN ----------------------------------- #
def main():
        # Open the Serial Port
        ser = serial.Serial(serial_com_port, baud_rate, timeout = None)

        # Log for debug
        csvfile = open('log.csv', 'w', newline='')
        csvwriter = csv.writer(csvfile, delimiter=',')
        csvwriter.writerow(['num_bytes_received', 'last_iter_val', 'read_val', 'bytes in buffer'])

        # flush serial buffer to delete any stored values
        #ser.set_buffer_size(rx_size = (2**31 - 1))
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        # read an initial starting value 
        last_iter_val = ord(ser.read(1))

        # init counters
        start_time = time.time()
        num_bytes_received = 0

        

        # read loop
        while ((time.time() - start_time) < 20):
                read_val_list = ser.read(size=2048)

                for read_val in read_val_list:
                        num_bytes_received += 1
                        csvwriter.writerow([num_bytes_received, last_iter_val, read_val, ser.in_waiting])

                        if (read_val == (1 + last_iter_val)):
                                last_iter_val = read_val
                        elif ((read_val == 0) and (last_iter_val == 64)):
                                last_iter_val = 0
                        else:
                                print(f'ERROR: last_iter_val:{last_iter_val}, read_val:{read_val}, num:{num_bytes_received}')
                                last_iter_val = read_val
        
        # calculate the bit rate 
        bit_rate = (num_bytes_received * 8) / (time.time() - start_time)
        print(f'bitrate:{bit_rate:.4f}')

        # Release the Serial Port and log file 
        csvfile.close()
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        ser.close()
        return
# ---------------------------------------------------------------------------- #

##########################
if __name__ == '__main__':
        main()
##########################