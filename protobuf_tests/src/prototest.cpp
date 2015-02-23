#include <string>
#include <iostream>
#include "proto/entity.pb.h"

int main(void)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    test::Component comp;
    comp.set_type(test::Component::COMP_0);
    std::string str("TEST");
    comp.add_data(str);

    std::cout << comp.DebugString();
    std::string serial;
    comp.SerializeToString(&serial);
    
    test::Component comp2;
    comp2.ParseFromString(serial);
    comp2.DebugString();

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
