'''
Author: Seth Sievers
Date: 4/21/24
Desc: A separate module to hold the global variables since python handles globals
        weirdly
'''

#! MUST ACCESS WITH cfg.<var_name> OR ELSE 

import threading
import ping

# ----------------------------- GLOBAL_VARIABLES ----------------------------- #
COM_STATE = 0
COM_PORT = None
SER = None 
PING_THREAD = threading.Thread(target=ping.ping, name='ping')
PRINT_LOCK = threading.Lock()
SER_LOCK = threading.Lock()
ARE_THREADS_ALIVE = True # will kill all threads when set false 
# ---------------------------------------------------------------------------- #