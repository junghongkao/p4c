#include <core.p4>

header Header {
    bit<32> data;
}

parser p1(packet_in p, out Header[2] h) {
    Header tmp_1;
    Header tmp_2;
    Header h_1;
    state start {
        h_1.setInvalid();
        p.extract<Header>(h_1);
        tmp_1 = h_1;
        h[0] = tmp_1;
        h_1.setInvalid();
        p.extract<Header>(h_1);
        tmp_2 = h_1;
        h[1] = tmp_2;
        transition accept;
    }
}

parser proto(packet_in p, out Header[2] h);
package top(proto _p);
top(p1()) main;
