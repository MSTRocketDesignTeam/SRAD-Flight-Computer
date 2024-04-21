'''
Author: Seth Sievers
Date: 4/21/24
Desc: A separate module to hold the global variables since python handles globals
        weirdly
'''

#! MUST ACCESS WITH cfg.<var_name> OR ELSE 

# ----------------------------- GLOBAL_VARIABLES ----------------------------- #
IS_CONNECTED = False # is the altimeter connected? 
COM_PORT = None
SER = None 
# ---------------------------------------------------------------------------- #