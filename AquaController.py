#!/usr/bin/python
# coding: utf-8

# In[32]:


import datetime
from subprocess import call


# In[33]:


now = datetime.datetime.now()
morning7am = now.replace(hour=7, minute=0, second=0, microsecond=0)
morning11am = now.replace(hour=11, minute=0, second=0, microsecond=0)
noon = now.replace(hour=12, minute=0, second=0, microsecond=0)
evening6pm = now.replace(hour=18, minute=0, second=0, microsecond=0)
night10pm = now.replace(hour=22, minute=0, second=0, microsecond=0)


# In[57]:


if now >= night10pm:
    print("Turn off all")
    call(["curl", "http://192.168.100.12/LightControll?state=0"])
    call(["curl", "http://192.168.100.12/FilterControll?state=0"])
    call(["curl", "http://192.168.100.12/Co2Controll?state=0"])
elif now >= evening6pm and now < night10pm :
    print("Keep only Filter and Light ON")
    call(["curl", "http://192.168.100.12/LightControll?state=1"])
    call(["curl", "http://192.168.100.12/FilterControll?state=1"])
    call(["curl", "http://192.168.100.12/Co2Controll?state=0"])
elif now >= noon and now < evening6pm:
    print("Keep Filter, Co2 and Light ON")
    call(["curl", "http://192.168.100.12/LightControll?state=1"])
    call(["curl", "http://192.168.100.12/FilterControll?state=1"])
    call(["curl", "http://192.168.100.12/Co2Controll?state=1"])
elif now >= morning11am and now < noon:
    print("Keep only Filter and Co2 ON")
    call(["curl", "http://192.168.100.12/LightControll?state=0"])
    call(["curl", "http://192.168.100.12/FilterControll?state=1"])
    call(["curl", "http://192.168.100.12/Co2Controll?state=1"])
elif now >= morning7am and now < morning11am:
    print("Keep only Filter ON")
    call(["curl", "http://192.168.100.12/LightControll?state=0"])
    call(["curl", "http://192.168.100.12/FilterControll?state=1"])
    call(["curl", "http://192.168.100.12/Co2Controll?state=0"])
else:
    print("All ON")
    call(["curl", "http://192.168.100.12/LightControll?state=1"])
    call(["curl", "http://192.168.100.12/FilterControll?state=1"])
    call(["curl", "http://192.168.100.12/Co2Controll?state=1"])

