#include "Club.h"

    Club::Club(const std::string& path) : file_path(path){}
    
    void Club::split_string(const std::string& str, std::vector<std::string>& words){
        std::istringstream iss(str);
        std::string word;
        for(;iss >> word;){
            words.push_back(word);
        }
        int space_counter = 0;
        for(auto chr : str){
            if(chr == ' '){
                ++space_counter;
            }
        }
        if(space_counter > words.size() - 1){
            throw std::runtime_error("to many spaces");
        }
    }
    std::string Club::size_t_to_date(size_t time){
        int hours = time / 60;
        int minutes = time % 60;
        std::string shours = std::to_string(hours);
        std::string sminutes = std::to_string(minutes);
        if(hours < 10){
            shours = '0' + shours;
        }
        if(minutes < 10){
            sminutes = '0' + sminutes;
        }
        return shours + ':' + sminutes;

    }
    size_t Club::parse_time(const std::string& str){
        int hours = 0;
        int minutes = 0;
        std::string tmp;
        for(int i = 0; i < 5; ++i){
            if(i == 2){
                continue;
            }
            tmp += str[i];
            if(i == 1){
                hours = std::stoi(tmp);
                if(hours >= 24){
                    throw std::runtime_error("incorrect hours format");
                }
                tmp = "";
            }
            if(i == 4){
                minutes = std::stoi(tmp);
                if(minutes >= 60){
                    throw std::runtime_error("incorrect minutes format");
                }
                tmp = "";
            }
            
        }

        return 60 * hours + minutes;
    }



    size_t Club::get_num(const std::string& str){
        std::regex pattern(R"(\b[1-9]\d*\b)");
        if(std::regex_match(str, pattern)){
            return std::stoi(str);
        }
        throw std::runtime_error("invalid number");
    }

    void Club::parse_club_time(const std::string& str){
        std::regex pattern(R"(\b([0-9]{2}):([0-9]{2})\b)");
        std::smatch matches;
        std::vector<std::string> timeMatches;

        std::string tmp_str = str;
        while (std::regex_search(tmp_str, matches, pattern)) {
            timeMatches.push_back(matches[0]);
            tmp_str = matches.suffix();
        }
        if(timeMatches.size() != 2){
            throw std::runtime_error("incorrect num of times");
        }

        this->start_time = parse_time(timeMatches[0]);
        this->end_time = parse_time(timeMatches[1]);

        if(this->end_time < this->start_time){
            this->work_time = 24 * 60 - this->start_time + this->end_time; 
        }
        else{
            this->work_time = this->end_time - this->start_time;
        }

        prev_event_time = this->start_time;

    }

    int Club::get_relative_time(int time){

        if(time < this->start_time){
            return 24 * 60 - start_time + time;
        }
        else{
            return time - this->start_time;
        }
        
    }

    bool Club::validate_time(const std::string& time){

        int tmp_time = get_relative_time(parse_time(time)); 

        if(tmp_time > this->work_time){
            return false;
        }

        return true;
    }


    bool Club::validate_event_args(const std::vector<std::string>& args){
        //checks time if its throws exception
        validate_time(args[0]);
        //wait for exception id
        int id = get_num(args[1]);
        //validates clients name
        std::regex pattern(R"([a-zA-Z0-9_-]+)");
        std::regex pattern_time(R"(\b([0-9]{2}):([0-9]{2})\b)");


        if(!std::regex_match(args[0], pattern_time)){
            return false;
        }
        if(!std::regex_match(args[2], pattern)){
            return false;
        }

        //wait for exception table
        if(args.size() == 4){
            int tab = get_num(args[3]);
            if(tab > this->table_num){
                return false;
            }
        }

        return true;

    }

    void Club::print_event(){
        for(const std::string & word : this->current_words){
            std::cout << word << " ";
        }
        std::cout << '\n';
    }

    bool Club::in_worktime(int time){
        
        if(time > this->work_time){
            return false;
        }

        return true;
    }

    void Club::end_workday(){

        for(const auto& client : this->all_clients){
            std::cout << size_t_to_date(this->end_time) << ' ' << 11 << ' ' << client.first << '\n';
        }

        std::cout << size_t_to_date(this->end_time) << '\n';
        this->tables.erase(0);
        for(const auto& table : this->tables){
            free_table(table.first, this->work_time);
            std::cout << table.first << ' ' << table.second.pay_hours * this->hour_cost 
            << ' ' << size_t_to_date(table.second.sum_minutes) << '\n';
        }
        day_finished = true;

    }

    bool Club::compare_time(const std::string& time){
        
        int rel_time = get_relative_time(parse_time(time));
        int prev_rel_time = get_relative_time(this->prev_event_time);
        
        if(rel_time < prev_rel_time && in_worktime(prev_rel_time) && in_worktime(rel_time)){
            return true;
        }
        

        if(rel_time < prev_rel_time && !in_worktime(rel_time) && !in_worktime(prev_rel_time) && !day_started){
            //TODO
            end_workday();
            return true;

        }

        if(rel_time > prev_rel_time && !in_worktime(rel_time) && in_worktime(prev_rel_time) && day_started){
            //TODO
            end_workday();
            return true;
        }

        return false;
    }


    void Club::free_table(int id, int time){
        if(id == 0){
            return;
        }
        if(!this->tables[id].occupied){
            return;
        }
        int used_time = time - this->tables[id].started_time;
        this->tables[id].sum_minutes += used_time;
        this->tables[id].pay_hours += used_time / 60;
        if(used_time % 60 != 0){
            this->tables[id].pay_hours += 1;
        }
        this->tables[id].occupied =false;
    }

    bool Club::set_table(const std::vector<std::string>& words){
        int table_id = get_num(words[3]);
        //table_id client wants to
        
        if(this->tables[table_id].occupied){
            return false;
        }
        else{
            
            int clients_id = this->all_clients[words[2]];

            free_table(clients_id, 
                        get_relative_time(parse_time(words[0])));


            if(waiting_clients.size() != 0 && clients_id != 0) {
                std::string first_client = this->waiting_clients.front();
                this->waiting_clients.pop_front(); 
                invoke_id12(words[0], first_client, this->all_clients[words[2]]);
            }

            this->all_clients[words[2]] = table_id;           
            this->tables[table_id].occupied = true;
            this->tables[table_id].started_time = get_relative_time(parse_time(words[0]));
            return true;
        }
    }

    bool Club::add_client(const std::string& client_name){
        if(this->all_clients.find(client_name) == this->all_clients.end()){
            this->all_clients[client_name] = 0;
            return true;
        }
        else {
            return false;
        }
    }

    //removes in all_client and waiting clients
    bool Club::remove_client(const std::string& client_name){
        if(this->all_clients.find(client_name) == this->all_clients.end()){
            return false; 
        }
        
        this->all_clients.erase(client_name);

        for(auto it = this->waiting_clients.begin(); it != this->waiting_clients.end(); ++it){
            if(*it == client_name){
                this->waiting_clients.erase(it);
                return true;
            }
        }
        return true;
    }

//time id name table
    void Club::invoke_error(const std::string& time, const std::string& msg){
        std::cout << time << ' ' << 13 << ' ' << msg << '\n';
    }

    void Club::invoke_id1(const std::vector<std::string>& words){

        if(words.size() != 3){
            throw std::runtime_error("incorrect num of arguments for id1 event");
        }

        if(!validate_time(words[0])){
            invoke_error(words[0], "NotOpenYet");
            return;
        }

        if(all_clients.find(words[2]) != all_clients.end()){
            invoke_error(words[0], "YouShallNotPass");
            return;
        }

        day_started = true;

        if(!add_client(words[2])){
            throw std::runtime_error("error adding client id1");
        }


    }

    void Club::invoke_id2(const std::vector<std::string>& words){
        if(!in_worktime(get_relative_time(parse_time(words[0])))){
            throw std::runtime_error("event not in work time");
        }
        day_started = true;
        if(words.size() != 4){
                throw std::runtime_error("incorrect num of arguments for id2 event");
        }

        if(all_clients.find(words[2]) == all_clients.end()){
            invoke_error(words[0], "ClientUnknown");
            return;
        }
        if(!set_table(words)){
            invoke_error(words[0], "PlaceIsBusy");
            return;
        }

    }

    void Club::invoke_id11(const std::vector<std::string>& words){
        if(!remove_client(words[2])){
            throw std::runtime_error("error removing client");
        }
        //this->all_clients.erase(words[2]);
        std::cout << words[0] << ' ' << 11 << ' ' << words[2] << '\n';
    }

    void Club::invoke_id3(const std::vector<std::string>& words){
        this->tables.erase(0);
        if(!in_worktime(get_relative_time(parse_time(words[0])))){
            throw std::runtime_error("event not in work time");
        }
        day_started = true;
        if(words.size() != 3){
                throw std::runtime_error("incorrect num of arguments for id3 event");
        }

        if(this->tables.size() == this->table_num){
            for(const auto& table : this->tables){
                if(!table.second.occupied){
                    invoke_error(words[0], "ICanWaitNoLonger!");
                    return;
                }
            }
        }
        else{
            invoke_error(words[0], "ICanWaitNoLonger!");
            return;
        }

        if(this->waiting_clients.size() > this->table_num){
            invoke_id11(words);
            return;
        }
            waiting_clients.push_back(words[2]);
        
    }
    
    void Club::invoke_id12(const std::string& time, const std::string& client, size_t table){
        set_table({time, "4", client, std::to_string(table)});
        this->all_clients[client] = table;
        std::cout << time << ' ' << 12 << ' ' << client << ' ' << table << '\n';

    }

    void Club::invoke_id4(const std::vector<std::string>& words){
        if(!in_worktime(get_relative_time(parse_time(words[0])))){
            throw std::runtime_error("event not in work time");
        }
        day_started = true;
        if(this->all_clients.find(words[2]) == this->all_clients.end()){
            invoke_error(words[0], "ClientUnknown");
            return;
        }

        size_t freed_table = this->all_clients[words[2]];
        free_table(freed_table, 
                        get_relative_time(parse_time(words[0])));

        //this->all_clients.erase(words[2]);
        if(!remove_client(words[2])){
            throw std::runtime_error("error to remove client");
        }

        if(this->waiting_clients.size() != 0 && freed_table != 0){
            std::string first_client = this->waiting_clients.front();
            this->waiting_clients.pop_front(); 
            invoke_id12(words[0], first_client, freed_table);
        }
        
    }

    




    //handles input events
    void Club::event_handler(const std::string& str){
        this->current_words.clear();
        split_string(str, this->current_words);
        if(current_words.size() != 3 && current_words.size() != 4){
            throw std::runtime_error("incorrect num of args in event");
            //return;
        }
        if(!validate_event_args(current_words)){
            throw std::runtime_error("incorrect args");
        }

        if(compare_time(current_words[0])){
            if(day_finished){
                return;
            }
            throw std::runtime_error("invalid timeline");
        }
        prev_event_time = parse_time(current_words[0]);


        print_event();
        
        size_t id = get_num(this->current_words[1]);
        switch (id) {
            case 1:
            {

                invoke_id1(this->current_words);
                return;
            }

            case 2:
            {
                invoke_id2(this->current_words);
                return;

            }
            case 3:
            {
                invoke_id3(this->current_words);
                return;

            }
            case 4:
            {
                invoke_id4(this->current_words);
                return;

            }
            default:
                throw std::runtime_error("incorrect id in switch case");
        
        }



        
    }


    void Club::start(){
        int str_cnt = 0;
        std::ifstream in(file_path);

        if(!in.is_open()){
            std::cerr << "file isn't opened\n";
            return;
        }
        std::string current_string;
        for (; std::getline(in, current_string);) {
            if (day_finished) {
                return;
            }
            try{
                if(str_cnt == 0){
                    this->table_num = get_num(current_string);
                }
                else if(str_cnt == 1){
                    parse_club_time(current_string);
                    std::cout << size_t_to_date(this->start_time) << '\n';
                }
                else if(str_cnt == 2){
                    this->hour_cost = get_num(current_string);
                }
                else{
                    event_handler(current_string);
                }

                ++str_cnt;
            }
            catch(std::runtime_error& exc){
                //std::cout << exc.what() << '\n';
                std::cout << '\n' << current_string;
                in.close();
                return; 
            } 
            catch (...) {
                //std::cout << "\nerror happend\n";
                std::cout << '\n' << current_string;
                in.close();
                return;
            }    
        
        }
        if(!day_finished){
            end_workday();
        }
        in.close();

    }
