#! /usr/bin/python3
import cgi, cgitb
import sys
import subprocess

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
adc_freq_hz  = int(form.getvalue('adc_freq_hz'))
tune_freq_hz  = int(form.getvalue('tune_freq_hz'))
streaming = form.getvalue('streaming')


# Send the result to the browser
print ("Content-type:text/html")
print()
print ("<html>")
print ('<head>')
print ("<title>Radio Configurator</title>")
print ('</head>')
print ('<body>')
print ("<h2>Radio Configurator</h2>")
print ("Setting up the radio now...")
print ("ADC Freq = %d, Tune Freq = %d" %(adc_freq_hz,tune_freq_hz))
adc_pinc = round(adc_freq_hz*(1<<27)/125e6)
tune_pinc = round(tune_freq_hz*(1<<27)/125e6)
print(f"ADC phase inc set to {adc_pinc}")
print(f"Tuner phase inc set to {tune_pinc}")
subprocess.run(['devmem', '0x43c00000', 'w', f"{adc_pinc}"])
subprocess.run(['devmem', '0x43c00004', 'w', f"{tune_pinc}"])
if (streaming == "streaming"):
    print ("streaming is Enabled<br>")
    subprocess.run(['devmem', '0x43c00008', 'w', '2'])
else :
    print ("streaming is Disabled<br>")
    subprocess.run(['devmem', '0x43c00008', 'w', '0'])
print ('</body>')
print ('</html>')

