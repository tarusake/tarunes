# Tang Primer 20K用のGowinプロジェクトをTclだけで構築する。
set script_dir [file dirname [file normalize [info script]]]
set repo_root [file normalize [file join $script_dir ..]]

set rom helloworld
if {[info exists ::env(ROM)] && $::env(ROM) ne ""} {
    set rom $::env(ROM)
}

proc require_file {path} {
    if {![file exists $path]} {
        error "Missing build input: $path"
    }
}

set prom_hex [file join $repo_root "${rom}_prg.hex"]
set crom_hex [file join $repo_root "${rom}_chr.hex"]
require_file $prom_hex
require_file $crom_hex
file copy -force $prom_hex [file join $script_dir rom_prg.hex]
file copy -force $crom_hex [file join $script_dir rom_chr.hex]

cd $script_dir
set_device -name GW2A-18C GW2A-LV18PG256C8/I7

foreach path [lsort [glob -nocomplain [file join $repo_root target *.sv]]] {
    add_file $path
}

foreach path [list \
    [file join $script_dir src gowin_rpll gowin_pll_27_to_135.v] \
    [file join $script_dir src gowin_tmds_output.sv] \
    [file join $script_dir src tang_primer_20k_top.sv] \
    [file join $script_dir src tang_primer_20k.cst] \
    [file join $script_dir src tang_primer_20k.sdc] \
] {
    require_file $path
    add_file $path
}

set_option -top_module tang_primer_20k_top
set_option -verilog_std sysv2017
set_option -use_sspi_as_gpio 1
set_option -use_done_as_gpio 1
set_option -use_ready_as_gpio 1
set_option -output_base_name tarunes_tangprimer20k
set_option -looplimit 2000
set_option -global_freq 100.000

puts "Building Tang Primer 20K bitstream with ROM: $rom"
run all
puts "Bitstream: [file join $script_dir impl pnr tarunes_tangprimer20k.fs]"
