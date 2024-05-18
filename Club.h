#include <cstddef>
#include <cstdlib>
#include <list>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <vector>

struct table{
    bool occupied = false;
    
    int pay_hours = 0;
    int sum_minutes = 0;

    int started_time = 0;
    table(){}
};

class Club
{
    private:
        size_t table_num = 0;

        size_t start_time = 0; //in minutes

        size_t end_time = 0; //in minutes

        size_t work_time = 0; //in minutes

        size_t hour_cost = 0;
        
        size_t prev_event_time = 0; //in minutes
        
        bool day_started = false;

        bool day_finished = false;

        std::vector<std::string> current_words;

        std::map<std::string, size_t> all_clients;

        std::list<std::string> waiting_clients;

        std::map<size_t, table> tables;

    private:

        std::string file_path = "";

    public:

        Club(const std::string& path);

    private:

        void split_string(const std::string& str, std::vector<std::string>& words);

        std::string size_t_to_date(size_t time);

        size_t parse_time(const std::string& str);

        size_t get_num(const std::string& str);

        void parse_club_time(const std::string& str);

        int get_relative_time(int time);

        bool validate_time(const std::string& time);

        bool validate_event_args(const std::vector<std::string>& args);

        void print_event();

        bool in_worktime(int time);

        void end_workday();

        bool compare_time(const std::string& time);


        void free_table(int id, int time);

        bool set_table(const std::vector<std::string>& words);

        bool add_client(const std::string& client_name);

        bool remove_client(const std::string& client_name);

    private:

        void invoke_error(const std::string& time, const std::string& msg);

        void invoke_id1(const std::vector<std::string>& words);

        void invoke_id2(const std::vector<std::string>& words);

        void invoke_id11(const std::vector<std::string>& words);

        void invoke_id3(const std::vector<std::string>& words);
            
        void invoke_id12(const std::string& time, const std::string& client, size_t table);

        void invoke_id4(const std::vector<std::string>& words);
    
        void event_handler(const std::string& str);

    public:
        //MAIN FUNCTION TO CALL
        void start();

};
