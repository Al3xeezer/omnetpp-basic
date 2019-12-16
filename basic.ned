
//
// Copyright (C) 2019 Jon Alesander Núñez
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


// Se definen los diferentes módulos de la red

simple sender
{
    gates:
        input inS;
        input in;
        output out;
}

simple receiver
{
    gates:
        input in;
        output out;
}

simple source
{
    gates:
        output out;
}


// Se realizan las conexiones entre los módulos

network basic
{
    types:
        channel Ethernet100 extends ned.DatarateChannel
        {
            datarate = 100Mbps;
            per = 0.01;
        }
    submodules:
        source: source {
            @display("p=45,152");
        }
        sender: sender {
            @display("p=136,152");
        }
        receiver: receiver {
            @display("p=279,152");
        }
    connections:
        source.out --> sender.inS;
        sender.out --> Ethernet100 --> receiver.in;
        receiver.out --> Ethernet100 --> sender.in;
        //sender.in <-- Ethernet100 <-- receiver.out;
        //receiver.in <-- Ethernet100 <-- sender.out;
}

