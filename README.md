# Window-Door-sensor-Version-5
Version 5 of the Window/Door sensor project this time with ultra low current consumption and a button input.

I have continued to develop the original window/Door sensor from the Youtube video of Makermeik (https://github.com/MakerMeik) and this is the latest version (version 5)
At each revision I've tried to reduce the power consumption more and more and with this version the Quiescent current with the Door/Window shut is a around 5.4uA and with the door open or using as a WiFi button (Doorbell etc.) then it's around 3.9uA and depending on usage you're looking at years of usage on a small capacity L-ion cell.

The trigger logic is essetially the same as the the original version except the output from the two XOR gates is inverted to give a high pulse and return to Low when stable, this enables the output to trigger the input of an OR gate which self latches to keep the enable pin of the SPX3189 LDO high and the ESP module powered up.

When the ESP has finished running its code it sets GPIO16 low which resets the OR gates latch and powers off the LDO.

To keep the power on time as short as possible I'm using ESP-NOW to send the data packets to the ESP receiver but you could connect to your router and send MQTT messages, this would use more power but as the consumption is quite low the cell would still last quite a long time.

It has been pointed out to me that there are dedicated ICs out there that can handle this sort of thing, wake up from a button at least, but I can't find them readily available and the ones I can find are quite expesive, more than the cost of a populated Version 5 PCB from JLCPCB anyway.
