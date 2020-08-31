#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/singleton.hpp>

using namespace std;
using namespace eosio;

CONTRACT gamblingdice : public contract {
  public:
    using contract::contract;
    
      gamblingdice( name receiver, name code, datastream<const char*> ds ) :
         // contract base class contructor
         contract(receiver, code, ds),
         // instantiate multi index instance as data member (find it defined below)
         _rooms(receiver, receiver.value),
         _messages(receiver, receiver.value),
         singleton_debug(receiver, receiver.value)
         {}
         

    ACTION hi(name from, name to, eosio::asset quantity, string message);
    ACTION clear();
    ACTION createroom(name player1, asset stake, uint64_t id);

    void deposit(const name from, const name to, const asset quantity, const std::string memo);
    void received(const eosio::name caller, eosio::name receiver, eosio::asset value, std::string memo);

      struct [[eosio::table]] testtable {
         uint64_t    primary_identifier;
         uint64_t    debug_uint1;
         checksum256 debug_checksum1;
         std::string debug_string1;
         uint64_t    primary_key() const { return primary_identifier; }
      } tt;

      using singleton_type = eosio::singleton<"testtable"_n, testtable>;
      singleton_type singleton_debug;

    TABLE messages {
      name    user;
      string  text;
      auto primary_key() const { return user.value; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
    
    TABLE rooms {
      uint64_t id;
      name    player1;
      name    player2;

      checksum256 entropy1;
      checksum256 entropy2;

      //eosio::asset   stake_test_delaration;
      //eosio::asset(0, symbol(symbol_code("SYS"),4)) stake; // Breaking change 1.2.x ->> 1.3.x
      asset   stake;

      auto primary_key() const { return id; }
    };
    typedef multi_index<name("rooms"), rooms> rooms_table;
    rooms_table _rooms;
    messages_table _messages;

  private:

    // Nothing private at the moment.
};
