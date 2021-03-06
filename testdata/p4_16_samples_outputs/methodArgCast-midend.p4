extern E {
    E();
    void setValue(in bit<32> arg);
}

control c() {
    @name("e") E() e;
    action act() {
        e.setValue(32w10);
    }
    table tbl_act() {
        actions = {
            act();
        }
        const default_action = act();
    }
    apply {
        tbl_act.apply();
    }
}

control proto();
package top(proto p);
top(c()) main;
