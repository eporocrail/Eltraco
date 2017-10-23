<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="8.2.2">
<drawing>
<settings>
<setting alwaysvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
<layer number="99" name="SpiceOrder" color="5" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="pinhead" urn="urn:adsk.eagle:library:325">
<description>&lt;b&gt;Pin Header Connectors&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="1X03" library_version="1">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-3.175" y1="1.27" x2="-1.905" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="1.27" x2="-1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="1.27" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="0.635" x2="1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="-0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="-1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="0.635" x2="-3.81" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="1.27" x2="-3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="-0.635" x2="-3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="-1.27" x2="-3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="1.27" y1="0.635" x2="1.905" y2="1.27" width="0.1524" layer="21"/>
<wire x1="1.905" y1="1.27" x2="3.175" y2="1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="1.27" x2="3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="3.81" y1="0.635" x2="3.81" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-0.635" x2="3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-1.27" x2="1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="1.905" y1="-1.27" x2="1.27" y2="-0.635" width="0.1524" layer="21"/>
<pad name="1" x="-2.54" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="2" x="0" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="3" x="2.54" y="0" drill="1.016" shape="long" rot="R90"/>
<text x="-3.8862" y="1.8288" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-3.81" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
<rectangle x1="-2.794" y1="-0.254" x2="-2.286" y2="0.254" layer="51"/>
<rectangle x1="2.286" y1="-0.254" x2="2.794" y2="0.254" layer="51"/>
</package>
<package name="1X03/90" library_version="1">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-3.81" y1="-1.905" x2="-1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-1.905" x2="-1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="0.635" x2="-3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="6.985" x2="-2.54" y2="1.27" width="0.762" layer="21"/>
<wire x1="-1.27" y1="-1.905" x2="1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="0.635" x2="-1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="0" y1="6.985" x2="0" y2="1.27" width="0.762" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-1.905" x2="3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="3.81" y1="0.635" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="2.54" y1="6.985" x2="2.54" y2="1.27" width="0.762" layer="21"/>
<pad name="1" x="-2.54" y="-3.81" drill="1.016" shape="long" rot="R90"/>
<pad name="2" x="0" y="-3.81" drill="1.016" shape="long" rot="R90"/>
<pad name="3" x="2.54" y="-3.81" drill="1.016" shape="long" rot="R90"/>
<text x="-4.445" y="-3.81" size="1.27" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="5.715" y="-3.81" size="1.27" layer="27" rot="R90">&gt;VALUE</text>
<rectangle x1="-2.921" y1="0.635" x2="-2.159" y2="1.143" layer="21"/>
<rectangle x1="-0.381" y1="0.635" x2="0.381" y2="1.143" layer="21"/>
<rectangle x1="2.159" y1="0.635" x2="2.921" y2="1.143" layer="21"/>
<rectangle x1="-2.921" y1="-2.921" x2="-2.159" y2="-1.905" layer="21"/>
<rectangle x1="-0.381" y1="-2.921" x2="0.381" y2="-1.905" layer="21"/>
<rectangle x1="2.159" y1="-2.921" x2="2.921" y2="-1.905" layer="21"/>
</package>
</packages>
<symbols>
<symbol name="PINHD3" library_version="1">
<wire x1="-6.35" y1="-5.08" x2="1.27" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="1.27" y1="-5.08" x2="1.27" y2="5.08" width="0.4064" layer="94"/>
<wire x1="1.27" y1="5.08" x2="-6.35" y2="5.08" width="0.4064" layer="94"/>
<wire x1="-6.35" y1="5.08" x2="-6.35" y2="-5.08" width="0.4064" layer="94"/>
<text x="-6.35" y="5.715" size="1.778" layer="95">&gt;NAME</text>
<text x="-6.35" y="-7.62" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-2.54" y="2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="2" x="-2.54" y="0" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="3" x="-2.54" y="-2.54" visible="pad" length="short" direction="pas" function="dot"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="PINHD-1X3" prefix="JP" uservalue="yes" library_version="1">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="A" symbol="PINHD3" x="0" y="0"/>
</gates>
<devices>
<device name="" package="1X03">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="/90" package="1X03/90">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="1.016" drill="0">
<clearance class="0" value="0.254"/>
</class>
</classes>
<parts>
<part name="HL1" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN1" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL2" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN2" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL3" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN3" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL4" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN4" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL5" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN5" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL6" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN6" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL7" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN7" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL8" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN8" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL9" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN9" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL10" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN10" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL11" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN11" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL12" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN12" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL13" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN13" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL14" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN14" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL15" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN15" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL16" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN16" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL17" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN17" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL18" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN18" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL19" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN19" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="HL20" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
<part name="CN20" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X3" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="HL1" gate="A" x="-12.7" y="96.52" rot="R90"/>
<instance part="CN1" gate="A" x="-12.7" y="83.82" rot="R90"/>
<instance part="HL2" gate="A" x="2.54" y="96.52" rot="R90"/>
<instance part="CN2" gate="A" x="2.54" y="83.82" rot="R90"/>
<instance part="HL3" gate="A" x="-12.7" y="68.58" rot="R90"/>
<instance part="CN3" gate="A" x="-12.7" y="55.88" rot="R90"/>
<instance part="HL4" gate="A" x="-12.7" y="33.02" rot="R90"/>
<instance part="CN4" gate="A" x="-12.7" y="20.32" rot="R90"/>
<instance part="HL5" gate="A" x="2.54" y="33.02" rot="R90"/>
<instance part="CN5" gate="A" x="2.54" y="20.32" rot="R90"/>
<instance part="HL6" gate="A" x="-12.7" y="5.08" rot="R90"/>
<instance part="CN6" gate="A" x="-12.7" y="-7.62" rot="R90"/>
<instance part="HL7" gate="A" x="27.94" y="96.52" rot="R90"/>
<instance part="CN7" gate="A" x="27.94" y="83.82" rot="R90"/>
<instance part="HL8" gate="A" x="43.18" y="96.52" rot="R90"/>
<instance part="CN8" gate="A" x="43.18" y="83.82" rot="R90"/>
<instance part="HL9" gate="A" x="27.94" y="68.58" rot="R90"/>
<instance part="CN9" gate="A" x="27.94" y="55.88" rot="R90"/>
<instance part="HL10" gate="A" x="27.94" y="33.02" rot="R90"/>
<instance part="CN10" gate="A" x="27.94" y="20.32" rot="R90"/>
<instance part="HL11" gate="A" x="43.18" y="33.02" rot="R90"/>
<instance part="CN11" gate="A" x="43.18" y="20.32" rot="R90"/>
<instance part="HL12" gate="A" x="27.94" y="5.08" rot="R90"/>
<instance part="CN12" gate="A" x="27.94" y="-7.62" rot="R90"/>
<instance part="HL13" gate="A" x="71.12" y="96.52" rot="R90"/>
<instance part="CN13" gate="A" x="71.12" y="83.82" rot="R90"/>
<instance part="HL14" gate="A" x="86.36" y="96.52" rot="R90"/>
<instance part="CN14" gate="A" x="86.36" y="83.82" rot="R90"/>
<instance part="HL15" gate="A" x="71.12" y="68.58" rot="R90"/>
<instance part="CN15" gate="A" x="71.12" y="55.88" rot="R90"/>
<instance part="HL16" gate="A" x="71.12" y="33.02" rot="R90"/>
<instance part="CN16" gate="A" x="71.12" y="20.32" rot="R90"/>
<instance part="HL17" gate="A" x="86.36" y="33.02" rot="R90"/>
<instance part="CN17" gate="A" x="86.36" y="20.32" rot="R90"/>
<instance part="HL18" gate="A" x="71.12" y="5.08" rot="R90"/>
<instance part="CN18" gate="A" x="71.12" y="-7.62" rot="R90"/>
<instance part="HL19" gate="A" x="104.14" y="96.52" rot="R90"/>
<instance part="CN19" gate="A" x="104.14" y="83.82" rot="R90"/>
<instance part="HL20" gate="A" x="119.38" y="96.52" rot="R90"/>
<instance part="CN20" gate="A" x="119.38" y="83.82" rot="R90"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$40" class="0">
<segment>
<pinref part="HL1" gate="A" pin="1"/>
<pinref part="CN1" gate="A" pin="1"/>
<wire x1="-15.24" y1="93.98" x2="-15.24" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$44" class="0">
<segment>
<pinref part="HL1" gate="A" pin="2"/>
<pinref part="CN1" gate="A" pin="2"/>
<wire x1="-12.7" y1="93.98" x2="-12.7" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$45" class="0">
<segment>
<pinref part="HL1" gate="A" pin="3"/>
<pinref part="CN1" gate="A" pin="3"/>
<wire x1="-10.16" y1="93.98" x2="-10.16" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$47" class="0">
<segment>
<pinref part="HL2" gate="A" pin="1"/>
<pinref part="CN2" gate="A" pin="1"/>
<wire x1="0" y1="93.98" x2="0" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$48" class="0">
<segment>
<pinref part="HL2" gate="A" pin="2"/>
<pinref part="CN2" gate="A" pin="2"/>
<wire x1="2.54" y1="93.98" x2="2.54" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$51" class="0">
<segment>
<pinref part="HL2" gate="A" pin="3"/>
<pinref part="CN2" gate="A" pin="3"/>
<wire x1="5.08" y1="93.98" x2="5.08" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$53" class="0">
<segment>
<pinref part="HL3" gate="A" pin="1"/>
<pinref part="CN3" gate="A" pin="1"/>
<wire x1="-15.24" y1="66.04" x2="-15.24" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$54" class="0">
<segment>
<pinref part="HL3" gate="A" pin="2"/>
<pinref part="CN3" gate="A" pin="2"/>
<wire x1="-12.7" y1="66.04" x2="-12.7" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$55" class="0">
<segment>
<pinref part="HL3" gate="A" pin="3"/>
<pinref part="CN3" gate="A" pin="3"/>
<wire x1="-10.16" y1="66.04" x2="-10.16" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$56" class="0">
<segment>
<pinref part="HL4" gate="A" pin="1"/>
<pinref part="CN4" gate="A" pin="1"/>
<wire x1="-15.24" y1="30.48" x2="-15.24" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$57" class="0">
<segment>
<pinref part="HL4" gate="A" pin="2"/>
<pinref part="CN4" gate="A" pin="2"/>
<wire x1="-12.7" y1="30.48" x2="-12.7" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$59" class="0">
<segment>
<pinref part="HL4" gate="A" pin="3"/>
<pinref part="CN4" gate="A" pin="3"/>
<wire x1="-10.16" y1="30.48" x2="-10.16" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$60" class="0">
<segment>
<pinref part="HL5" gate="A" pin="1"/>
<pinref part="CN5" gate="A" pin="1"/>
<wire x1="0" y1="30.48" x2="0" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$61" class="0">
<segment>
<pinref part="HL5" gate="A" pin="2"/>
<pinref part="CN5" gate="A" pin="2"/>
<wire x1="2.54" y1="30.48" x2="2.54" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$62" class="0">
<segment>
<pinref part="HL5" gate="A" pin="3"/>
<pinref part="CN5" gate="A" pin="3"/>
<wire x1="5.08" y1="30.48" x2="5.08" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$63" class="0">
<segment>
<pinref part="HL6" gate="A" pin="1"/>
<pinref part="CN6" gate="A" pin="1"/>
<wire x1="-15.24" y1="2.54" x2="-15.24" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$64" class="0">
<segment>
<pinref part="HL6" gate="A" pin="2"/>
<pinref part="CN6" gate="A" pin="2"/>
<wire x1="-12.7" y1="2.54" x2="-12.7" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$65" class="0">
<segment>
<pinref part="HL6" gate="A" pin="3"/>
<pinref part="CN6" gate="A" pin="3"/>
<wire x1="-10.16" y1="2.54" x2="-10.16" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="HL7" gate="A" pin="1"/>
<pinref part="CN7" gate="A" pin="1"/>
<wire x1="25.4" y1="93.98" x2="25.4" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="HL7" gate="A" pin="2"/>
<pinref part="CN7" gate="A" pin="2"/>
<wire x1="27.94" y1="93.98" x2="27.94" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="HL7" gate="A" pin="3"/>
<pinref part="CN7" gate="A" pin="3"/>
<wire x1="30.48" y1="93.98" x2="30.48" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="HL8" gate="A" pin="1"/>
<pinref part="CN8" gate="A" pin="1"/>
<wire x1="40.64" y1="93.98" x2="40.64" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="HL8" gate="A" pin="2"/>
<pinref part="CN8" gate="A" pin="2"/>
<wire x1="43.18" y1="93.98" x2="43.18" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="HL8" gate="A" pin="3"/>
<pinref part="CN8" gate="A" pin="3"/>
<wire x1="45.72" y1="93.98" x2="45.72" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="HL9" gate="A" pin="1"/>
<pinref part="CN9" gate="A" pin="1"/>
<wire x1="25.4" y1="66.04" x2="25.4" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="HL9" gate="A" pin="2"/>
<pinref part="CN9" gate="A" pin="2"/>
<wire x1="27.94" y1="66.04" x2="27.94" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="HL9" gate="A" pin="3"/>
<pinref part="CN9" gate="A" pin="3"/>
<wire x1="30.48" y1="66.04" x2="30.48" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="HL10" gate="A" pin="1"/>
<pinref part="CN10" gate="A" pin="1"/>
<wire x1="25.4" y1="30.48" x2="25.4" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="HL10" gate="A" pin="2"/>
<pinref part="CN10" gate="A" pin="2"/>
<wire x1="27.94" y1="30.48" x2="27.94" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="HL10" gate="A" pin="3"/>
<pinref part="CN10" gate="A" pin="3"/>
<wire x1="30.48" y1="30.48" x2="30.48" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="HL11" gate="A" pin="1"/>
<pinref part="CN11" gate="A" pin="1"/>
<wire x1="40.64" y1="30.48" x2="40.64" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="HL11" gate="A" pin="2"/>
<pinref part="CN11" gate="A" pin="2"/>
<wire x1="43.18" y1="30.48" x2="43.18" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="HL11" gate="A" pin="3"/>
<pinref part="CN11" gate="A" pin="3"/>
<wire x1="45.72" y1="30.48" x2="45.72" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<pinref part="HL12" gate="A" pin="1"/>
<pinref part="CN12" gate="A" pin="1"/>
<wire x1="25.4" y1="2.54" x2="25.4" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$17" class="0">
<segment>
<pinref part="HL12" gate="A" pin="2"/>
<pinref part="CN12" gate="A" pin="2"/>
<wire x1="27.94" y1="2.54" x2="27.94" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<pinref part="HL12" gate="A" pin="3"/>
<pinref part="CN12" gate="A" pin="3"/>
<wire x1="30.48" y1="2.54" x2="30.48" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$19" class="0">
<segment>
<pinref part="HL13" gate="A" pin="1"/>
<pinref part="CN13" gate="A" pin="1"/>
<wire x1="68.58" y1="93.98" x2="68.58" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$20" class="0">
<segment>
<pinref part="HL13" gate="A" pin="2"/>
<pinref part="CN13" gate="A" pin="2"/>
<wire x1="71.12" y1="93.98" x2="71.12" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$21" class="0">
<segment>
<pinref part="HL13" gate="A" pin="3"/>
<pinref part="CN13" gate="A" pin="3"/>
<wire x1="73.66" y1="93.98" x2="73.66" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$22" class="0">
<segment>
<pinref part="HL14" gate="A" pin="1"/>
<pinref part="CN14" gate="A" pin="1"/>
<wire x1="83.82" y1="93.98" x2="83.82" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$23" class="0">
<segment>
<pinref part="HL14" gate="A" pin="2"/>
<pinref part="CN14" gate="A" pin="2"/>
<wire x1="86.36" y1="93.98" x2="86.36" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$24" class="0">
<segment>
<pinref part="HL14" gate="A" pin="3"/>
<pinref part="CN14" gate="A" pin="3"/>
<wire x1="88.9" y1="93.98" x2="88.9" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$25" class="0">
<segment>
<pinref part="HL15" gate="A" pin="1"/>
<pinref part="CN15" gate="A" pin="1"/>
<wire x1="68.58" y1="66.04" x2="68.58" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$26" class="0">
<segment>
<pinref part="HL15" gate="A" pin="2"/>
<pinref part="CN15" gate="A" pin="2"/>
<wire x1="71.12" y1="66.04" x2="71.12" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$27" class="0">
<segment>
<pinref part="HL15" gate="A" pin="3"/>
<pinref part="CN15" gate="A" pin="3"/>
<wire x1="73.66" y1="66.04" x2="73.66" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$28" class="0">
<segment>
<pinref part="HL16" gate="A" pin="1"/>
<pinref part="CN16" gate="A" pin="1"/>
<wire x1="68.58" y1="30.48" x2="68.58" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$29" class="0">
<segment>
<pinref part="HL16" gate="A" pin="2"/>
<pinref part="CN16" gate="A" pin="2"/>
<wire x1="71.12" y1="30.48" x2="71.12" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$30" class="0">
<segment>
<pinref part="HL16" gate="A" pin="3"/>
<pinref part="CN16" gate="A" pin="3"/>
<wire x1="73.66" y1="30.48" x2="73.66" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$31" class="0">
<segment>
<pinref part="HL17" gate="A" pin="1"/>
<pinref part="CN17" gate="A" pin="1"/>
<wire x1="83.82" y1="30.48" x2="83.82" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$32" class="0">
<segment>
<pinref part="HL17" gate="A" pin="2"/>
<pinref part="CN17" gate="A" pin="2"/>
<wire x1="86.36" y1="30.48" x2="86.36" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$33" class="0">
<segment>
<pinref part="HL17" gate="A" pin="3"/>
<pinref part="CN17" gate="A" pin="3"/>
<wire x1="88.9" y1="30.48" x2="88.9" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$34" class="0">
<segment>
<pinref part="HL18" gate="A" pin="1"/>
<pinref part="CN18" gate="A" pin="1"/>
<wire x1="68.58" y1="2.54" x2="68.58" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$35" class="0">
<segment>
<pinref part="HL18" gate="A" pin="2"/>
<pinref part="CN18" gate="A" pin="2"/>
<wire x1="71.12" y1="2.54" x2="71.12" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$36" class="0">
<segment>
<pinref part="HL18" gate="A" pin="3"/>
<pinref part="CN18" gate="A" pin="3"/>
<wire x1="73.66" y1="2.54" x2="73.66" y2="-10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$37" class="0">
<segment>
<pinref part="HL19" gate="A" pin="1"/>
<pinref part="CN19" gate="A" pin="1"/>
<wire x1="101.6" y1="93.98" x2="101.6" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$38" class="0">
<segment>
<pinref part="HL19" gate="A" pin="2"/>
<pinref part="CN19" gate="A" pin="2"/>
<wire x1="104.14" y1="93.98" x2="104.14" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$39" class="0">
<segment>
<pinref part="HL19" gate="A" pin="3"/>
<pinref part="CN19" gate="A" pin="3"/>
<wire x1="106.68" y1="93.98" x2="106.68" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$41" class="0">
<segment>
<pinref part="HL20" gate="A" pin="1"/>
<pinref part="CN20" gate="A" pin="1"/>
<wire x1="116.84" y1="93.98" x2="116.84" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$42" class="0">
<segment>
<pinref part="HL20" gate="A" pin="2"/>
<pinref part="CN20" gate="A" pin="2"/>
<wire x1="119.38" y1="93.98" x2="119.38" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$43" class="0">
<segment>
<pinref part="HL20" gate="A" pin="3"/>
<pinref part="CN20" gate="A" pin="3"/>
<wire x1="121.92" y1="93.98" x2="121.92" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, Eagle supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
</compatibility>
</eagle>
