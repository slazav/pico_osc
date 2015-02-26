#!/usr/bin/wish
package require http
package require Img
package require base64

pack [entry .e -textvar e] -fill x
bind .e <Return> {go .f.t $e}
pack [frame .f] -fill both -expand 1
pack [scrollbar .f.y -command ".f.t yview"] -fill y -side right
pack [text .f.t -wrap word -yscrollc ".f.y set"] \
   -fill both -expand 1 -side right
foreach i {red blue green3} {.f.t tag config $i -foreground $i}
focus .e
raise .

proc go {w url} {
   set token [http::geturl $url]
   upvar #0 $token arr
   $w insert end \n$arr(url)   blue
   if [info exists arr(error)] {$w insert end \n$arr(error) red}
   foreach {tag value} $arr(meta) {
       $w insert end \n$tag\t$value green3
   }
   $w see end
   if [regexp {Content-Type image/([^ ]+)} $arr(meta) -> format] {
       set im [image create photo -data [base64::encode $arr(body)]]
       $w insert end \n
       $w image create end -image $im
   } else {
       $w insert end \n$arr(body)
   }
   http::cleanup $token
}
bind . <Escape> {exec wish $argv0 &; exit}
