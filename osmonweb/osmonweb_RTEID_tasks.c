/*
 * file: osmonweb_RTEID_tasks.c
 * Do not edit here!
 */
 #include "osmonweb_RTEID_tasks.h"
   const char osmonweb_RTEID_task_title[] =
      "\n"
      "<head>\n"
      "<!-- start of document title here -->\n"
      "   <title>RTEMS Classic (RTEID) API Tasks on <!--%"
      "printstr %s|4 --></title>\n"
      "   <meta http-equiv=\"cache-control\" content=\"no"
      "-cache\">\n"
      "   <meta http-equiv=\"refresh\" content=\"<!--%pri"
      "ntint %d|7-->; URL=/goforms/osmonweb?<!--%printstr"
      " %s|12-->\">\n"
      "<!-- end of document title -->\n"
      "</head>\n"
      "<body>\n"
   ;

   const char osmonweb_RTEID_task_header[] =
      "\n"
      "<table border=\"0\" cellspacing=\"0\" cellpadding="
      "\"5\" width=\"100%\">\n"
      "   <tr><td>\n"
      "   <div align=\"center\">\n"
      "   <!-- start of API selection bar -->\n"
      "   <table border=\"0\" cellspacing=\"0\" width=\"1"
      "00%\">\n"
      "      <tr valign=\"center\">\n"
      "      <td width=\"20\">&nbsp;</td>\n"
      "      <td bgcolor=\"#4040ff\" align=\"center\" wid"
      "th=\"200\">\n"
      "         <a href=\"/goforms/osmonweb?api=RTEID\" n"
      "ame=\"RTEID objects\">\n"
      "         <font color=#ffffff size=\"+1\"><b>Classi"
      "c (RTEID) API</b></font>\n"
      "         </a>\n"
      "      </td>\n"
      "      <td width=\"20\">&nbsp;</td>\n"
      "      <td bgcolor=\"#E0E0E0\" align=\"center\" wid"
      "th=\"200\">\n"
      "         <a href=\"/goforms/osmonweb?api=POSIX\" n"
      "ame=\"POSIX objects\">\n"
      "         <font color=\"Black\" size=\"+1\">POSIX A"
      "PI</font>\n"
      "         </a>\n"
      "      </td>\n"
      "      <td width=\"20\">&nbsp;</td>\n"
      "      <td bgcolor=\"#E0E0E0\" align=\"center\" wid"
      "th=\"200\">\n"
      "         <a href=\"/goforms/osmonweb?api=ITRON\" n"
      "ame=\"ITRON objects\">\n"
      "         <font color=\"Black\" size=\"+1\">ITRON A"
      "PI</font>\n"
      "         </a>\n"
      "      </td>\n"
      "      <td width=\"20\">&nbsp;</td>\n"
      "      </tr>\n"
      "      <tr><td colspan=10 bgcolor=\"#4040ff\"><img "
      "width=1 height=1 alt=\"\"></td></tr>\n"
      "   </table>\n"
      "   <!-- end of API selection bar -->\n"
      "\n"
      "   <!-- start of Object selection bar -->\n"
      "   <table border=\"0\"  cellspacing=\"0\" width=\""
      "80%\">\n"
      "      <tr>\n"
      "         <td width=\"20\">&nbsp;</td>\n"
      "         <td bgcolor=\"#4040ff\" align=\"center\" "
      "width=\"200\">\n"
      "            <a href=\"/goforms/osmonweb?api=RTEID&"
      "obj_type=task\" name=\"RTEID tasks\">\n"
      "            <font color=#ffffff><b>Tasks</b></font"
      ">\n"
      "            </a></td>\n"
      "         <td width=\"20\">&nbsp;</td>\n"
      "         <td bgcolor=\"#8080E0\" align=\"center\" "
      "width=\"200\">\n"
      "            <a href=\"/goforms/osmonweb?api=RTEID&"
      "obj_type=sema\" name=\"RTEID semaphores\">\n"
      "            <font color=\"Black\">Semaphores</font"
      ">\n"
      "            </a></td>\n"
      "         <td width=\"20\">&nbsp;</td>\n"
      "         <td bgcolor=\"#8080E0\" align=\"center\" "
      "width=\"200\">\n"
      "            <a href=\"/goforms/osmonweb?api=RTEID&"
      "obj_type=queue\" name=\"RTEID Message Queues\">\n"
      "            <font color=\"Black\">Message Queues</"
      "font>\n"
      "            </a></td>\n"
      "         <td width=\"20\">&nbsp;</td>\n"
      "         <td bgcolor=\"#8080E0\" align=\"center\" "
      "width=\"200\">\n"
      "            <a href=\"/goforms/osmonweb?api=RTEID&"
      "obj_type=rmon\" name=\"RTEID Rate Monotonic Timers"
      "\">\n"
      "            <font color=\"Black\">Rate Monotonic T"
      "imers</font>\n"
      "            </a></td>\n"
      "         <td width=\"20\">&nbsp;</td>\n"
      "         <td bgcolor=\"#8080E0\" align=\"center\" "
      "width=\"200\">\n"
      "            <a href=\"/goforms/osmonweb?api=RTEID&"
      "obj_type=part\" name=\"RTEID Partitions\">\n"
      "            <font color=\"Black\">Partitions</font"
      ">\n"
      "            </a></td>\n"
      "         <td width=\"20\">&nbsp;</td>\n"
      "         <td bgcolor=\"#8080E0\" align=\"center\" "
      "width=\"200\">\n"
      "            <a href=\"/goforms/osmonweb?api=RTEID&"
      "obj_type=region\" name=\"RTEID Regions\">\n"
      "            <font color=\"Black\">Regions</font>\n"
      "            </a></td>\n"
      "         <td width=\"20\">&nbsp;</td>\n"
      "      </tr>\n"
      "   </table>\n"
      "<!-- end of Object selection bar -->\n"
      "   </div>\n"
      "   </td></tr><tr><td>\n"
      "      <div align=\"center\">\n"
      "<!-- start of document title here -->\n"
      "      <br><h2>Classic (RTEID) API Tasks On Target "
      "<b><font color=\"#4040ff\"><!--%printstr %s|4 --><"
      "/font></b></h2><br>\n"
      "<!-- end of document title -->\n"
      "      </div>\n"
   ;

   const char osmonweb_RTEID_task_tabhead[] =
      "\n"
      "<!-- start of object table -->\n"
      "      <table border=\"1\"width=100%>\n"
      "         <tr valign=\"top\" bgcolor=\"#bfbfbf\">\n"
      "            <td align=\"center\">\n"
      "               <b>ID</b><br>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=1&sort_select=0\"   name="
      "\"lowest ID first\" >      <IMG src=\"/osmonweb/ar"
      "row-N-S.gif\" width=24 height=24 border=0></a>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=-1&sort_select=0\"  name="
      "\"highest ID first\">      <IMG src=\"/osmonweb/ar"
      "row-S-N.gif\" width=24 height=24 border=0></a>\n"
      "            </td><td align=\"center\">\n"
      "               <b>Name</b><br>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=1&sort_select=1\"   name="
      "\"lowest Name first\" >    <IMG src=\"/osmonweb/ar"
      "row-N-S.gif\" width=24 height=24 border=0></a>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=-1&sort_select=1\"  name="
      "\"highest Name first\">    <IMG src=\"/osmonweb/ar"
      "row-S-N.gif\" width=24 height=24 border=0></a>\n"
      "            </td><td align=\"center\">\n"
      "               <b>Priority</b><br>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=-1&sort_select=6\"  name="
      "\"lowest proritiy first\" ><IMG src=\"/osmonweb/ar"
      "row-N-S.gif\" width=24 height=24 border=0></a>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=1&sort_select=6\"   name="
      "\"highest priority first\"><IMG src=\"/osmonweb/ar"
      "row-S-N.gif\" width=24 height=24 border=0></a>\n"
      "            </td><td align=\"center\">\n"
      "               <b>State</b><br>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=1&sort_select=7\"   name="
      "\"lowest state first\"    ><IMG src=\"/osmonweb/ar"
      "row-N-S.gif\" width=24 height=24 border=0></a>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=-1&sort_select=7\"  name="
      "\"highest state first\"   ><IMG src=\"/osmonweb/ar"
      "row-S-N.gif\" width=24 height=24 border=0></a>\n"
      "            </td><td align=\"center\">\n"
      "               <b>Ticks</b><br>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=1&sort_select=13\"  name="
      "\"lowest ticks first\"    ><IMG src=\"/osmonweb/ar"
      "row-N-S.gif\" width=24 height=24 border=0></a>\n"
      "               <a href=\"/goforms/osmonweb?<!--%pr"
      "intstr %s|12-->&sort_dir=-1&sort_select=13\" name="
      "\"highest ticks first\"   ><IMG src=\"/osmonweb/ar"
      "row-S-N.gif\" width=24 height=24 border=0></a>\n"
      "         </td></tr>\n"
   ;

   const char osmonweb_RTEID_task_tabentry[] =
      "\n"
      "         <tr align=\"right\">\n"
      "            <td><div align=\"center\"><a href=\"/g"
      "oforms/osmonweb?api=RTEID&obj_type=task&obj_curr=0"
      "x<!--%printint %08x|0-->\"\n"
      "                   title=\"Details for 0x<!--%prin"
      "tint %08x|0-->\">\n"
      "               0x<!--%printint %08x|0-->\n"
      "            </a></div></td>\n"
      "            <td><b><div align=\"center\">\n"
      "               <a href=\"/goforms/osmonweb?api=RTE"
      "ID&obj_type=task&obj_curr=0x<!--%printint %08x|0--"
      ">\" title=\"Details for <!--%printname %s|1 -->\">"
      "\n"
      "               <code><!--%printname %s|1 --></code"
      "></a>\n"
      "            </div></b></td>\n"
      "     <!-- interpolate color from red (high) to azu"
      "r (low) priority -->\n"
      "            <td bgcolor=\"<!--%printipol #%02X|6|0"
      "|255|0xFF|0x40--><!--%printipol %02X|6|0|255|0x40|"
      "0xFF--><!--%printipol %02X|6|0|255|0x40|0xFF-->\">"
      "\n"
      "        <!--%printint %d|6--></td>\n"
      "            <td bgcolor=\"<!--%printselect %s|7|0x"
      "ffffffff|#808080|#60FF60|#4040C0|#FF8000|#E0E000|#"
      "FF0000-->\" align=\"center\">\n"
      "                         <!--%printselect %s|7|0xf"
      "fffffff|       |Ready|Dormant|Suspended|Delayed|Bl"
      "ocked--></td>\n"
      "            <td bgcolor=\"#<!--%printipol %02X|14|"
      "0|1000|0x40|0xFF--><!--%printipol %02X|14|0|1000|0"
      "xFF|0x40--><!--%printipol %02X|14|0|1000|0xFF|0x40"
      "-->\">\n"
      "        <!--%printint %d|13--></td>\n"
      "         </tr>\n"
   ;

   const char osmonweb_RTEID_task_tabfoot[] =
      "\n"
      "      </table>\n"
      "<!-- end of object table -->\n"
      "   </td></tr>\n"
      "   <tr><td>\n"
   ;

   const char osmonweb_RTEID_task_footer[] =
      "\n"
      "<hr>\n"
      "<!-- start of general display options -->\n"
      "  <h2><div align=\"center\">Display Options</div><"
      "/h2>\n"
      "   <div align=\"center\"><table>\n"
      "      <form action=\"/goforms/osmonweb?<!--%prints"
      "tr %s|12-->&auto_rfsh_enab=0&hdr_rep_enable=0&dumm"
      "y=dummy\" method=\"POST\" enctype=\"application/x-"
      "www-form-urlencoded\">\n"
      "      <tr>\n"
      "         <td><input type=\"checkbox\" <!--%printse"
      "lect %s|6|0xffffffff|||checked--> name=\"auto_rfsh"
      "_enab\" value=\"1\">autorefresh </td>\n"
      "         <td>every <input type=\"text\" name=\"aut"
      "o_rfsh_time\" value=<!--%printint %d|7--> size=10 "
      "maxlength=10> seconds</td>\n"
      "      </tr>\n"
      "      <tr>\n"
      "         <td><input type=\"checkbox\" <!--%printse"
      "lect %s|10|0xffffffff|||checked--> name=\"hdr_rep_"
      "enable\" value=\"1\">repeat header </td>\n"
      "         <td>every <input type=\"text\" name=\"hdr"
      "_rep_cnt\" value=<!--%printint %d|11--> size=3 max"
      "length=3> lines</td>\n"
      "      </tr>\n"
      "      <tr>\n"
      "         <td><input type=\"submit\" name=\"Submit\""
      " value=\"submit\"></td>\n"
      "      </tr></form>\n"
      "   </table></div>\n"
      "<hr>\n"
      "<div align=\"center\"><font size=\"-1\"><font  col"
      "or=\"blue\"><i>osmonweb</i></font> for RTEMS imple"
      "mented by IMD</font><br><IMG src=\"/osmonweb/imdlo"
      "go_sm.jpg\" width=42 height=21 border=0></div>\n"
      "<!-- end of general display options -->\n"
      "    </td></tr>\n"
      "</table>\n"
   ;

   const char osmonweb_RTEID_task_trailer[] =
      "\n"
      "</body>\n"
   ;

