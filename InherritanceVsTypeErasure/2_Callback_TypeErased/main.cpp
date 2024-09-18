#include <iostream>
#include <functional>

namespace DB
{
    using GetAddressFunction = std::function< std::string() >;
    struct System
    {
        void Execute( const GetAddressFunction& getAddress )
        {
            std::cout << getAddress() << std::endl;
        }        
    };
}

namespace Application
{
    std::string GetAddress() { return "hello world"; };
}

int main()
{
    DB::System db;
    db.Execute( Application::GetAddress );
    return 0;
}