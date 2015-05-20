# osc_interface class

package require Itcl
package require http
namespace eval pscope {}

itcl::class pscope::interface {
  public common addr     localhost:8081

  method run_cmd {cmd args} {
    set args [join $args &]
    # puts "$addr/$cmd?$args"
    return [http::geturl "$addr/$cmd?$args"]
  }
}
