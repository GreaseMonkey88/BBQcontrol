# BBQcontrol
### More beer - less turning knobs

Mechanical controller with 3D-printed parts to control your BBQ over WiFi. The device uses a ESP8266, a servo and a buck converter for powering the device. The 3D-printed knob fits on a Weber Spirit E320 from 2014. Device control over MQTT.

<img src="Doc/bbq.png" width="300">

### BOM
- ESP8266 or compatible, I use a Wemos D1 mini clone
- DC-DC buck converter LM2596 [AliExpress link](https://www.aliexpress.com/item/32792186556.html?spm=a2g0s.9042311.0.0.27424c4dV3YrsG)
- XT30 connector [AliExpress link](https://www.aliexpress.com/item/32773333544.html?spm=a2g0s.9042311.0.0.27424c4dV3YrsG)
- Servo DS3218 control angle 270° [Amazon link](https://www.amazon.de/gp/product/B07Q65JY1K/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1)
- 3D-printed parts, download from Thingiverse [here](https://www.thingiverse.com/thing:4780922)
- 4x neodyme magnets 10x3mm


### Power supply
I use a battery from my power drill, 18V 2.5Ah, which lasts probably more than 2 days.  If you have the same type of battery you can download 3D-printable adapter [here on Thingiverse](https://www.thingiverse.com/thing:4244545)
