/**
 * @file
 * @copyright  Copyright 2016 GNSS Sensor Ltd. All right reserved.
 * @author     Sergey Khabarov - sergeykhbr@gmail.com
 * @brief      Multi-port CPU Integer Registers memory.
 */

#include "regibank.h"

namespace debugger {

RegIntBank::RegIntBank(sc_module_name name_) : sc_module(name_) {
    SC_METHOD(comb);
    sensitive << i_nrst;
    sensitive << i_radr1;
    sensitive << i_radr2;
    sensitive << i_wena;
    sensitive << i_wdata;
    sensitive << i_waddr;
    sensitive << i_dport_ena;
    sensitive << i_dport_write;
    sensitive << i_dport_addr;
    sensitive << i_dport_wdata;
    sensitive << r.update;

    SC_METHOD(registers);
    sensitive << i_clk.pos();
};

void RegIntBank::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    if (o_vcd) {
        sc_trace(o_vcd, i_wena, "/top/proc0/regs/i_wena");
        sc_trace(o_vcd, i_waddr, "/top/proc0/regs/i_waddr");
        sc_trace(o_vcd, i_wdata, "/top/proc0/regs/i_wdata");
        sc_trace(o_vcd, r.mem[5], "/top/proc0/regs/r4");
        sc_trace(o_vcd, o_rdata1, "/top/proc0/regs/o_rdata1");
        sc_trace(o_vcd, o_rdata2, "/top/proc0/regs/o_rdata2");
        sc_trace(o_vcd, o_dport_rdata, "/top/proc0/regs/o_dport_rdata");
    }
}

void RegIntBank::comb() {
    v = r;

    /** Debug port has higher priority. Collision must be controlled by SW */
    if (i_dport_ena.read() && i_dport_write.read()) {
        if (i_dport_addr.read() != 0) {
            v.mem[i_dport_addr.read()] = i_dport_wdata;
        }
    } else if (i_wena.read()) {
        if (i_waddr.read() != 0) {
            v.mem[i_waddr.read()] = i_wdata;
        }
    }
    v.update = !r.update.read();

    if (!i_nrst.read()) {   
        v.mem[0] = 0;
        for (int i = 1; i < Reg_Total; i++) {
            v.mem[i] = 0xfeedface;
        }
        v.update = 0;
    }

    o_rdata1 = r.mem[i_radr1.read()];
    o_rdata2 = r.mem[i_radr2.read()];
    o_dport_rdata = r.mem[i_dport_addr.read()];
    o_ra = r.mem[Reg_ra];
}

void RegIntBank::registers() {
    r = v;
}

}  // namespace debugger

