# CANBUFFER ver1a
  * Can be used in conjunction with CANGATE as an allternative way to produce a 'self consumed' event
  * Outputs of CANGATE can be fed into inputs
  * Setting Event variable 1 to 255 will invert the output event
  * Node Variable 1 is used to either use the same incoming event number  or to multiply the event number by 10
  * Setting NV1 to 1 - same event number in and our e.g. input 210 output 210
  * Setting NV1 to 10 multiplies the event number by 10 e.g. input 210 output 2100
  * Philip Silver MERG Member 4082
