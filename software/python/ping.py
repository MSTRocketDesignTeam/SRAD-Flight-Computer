'''
Author: Seth Sievers
Date: 4/21/24
Desc: This function is designed to periodically send a 'ping' command (0xCC) to the
        SRAD altimeter so that it keeps its serial connection alive. 
'''

import cfg
import time
import threading 

# ----------------------------------- PING ----------------------------------- #
def ping(): 
        while (cfg.ARE_THREADS_ALIVE):
                # will wait for 2s in between sending 'ping' (0xCC)
                time.sleep(2)

                # acquire serial lock and send 
                if (cfg.IS_CONNECTED):
                        with cfg.SER_LOCK:
                                cfg.SER.write(bytes([0xCC]))
                                with cfg.PRINT_LOCK:
                                        print('test')
        return
# ---------------------------------------------------------------------------- #
