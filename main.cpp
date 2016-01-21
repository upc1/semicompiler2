//UPC ;-)
//group work Saman A.Mirhosseni and UPC
//semi compiler ---- c programming 94 final project
//no copyright low ---> copy it for free !!! but: please do not delete UPC
#include <iostream>
#include <fstream>//for reading from file and writing on file
#include <string>//for string usage instant of char *
#include <windows.h>//for debug usage not in compiler
#include <conio.h>//for debug usage not in compiler
using namespace std;
int brace_do_not_owner[3000]={0};//for {} which does not has a owner-->set all to 0--->use in the whole program
//use for save each brace owner too!
int brace_temp[3000]={0};//use for make brace_do_not_owner in recursive
int brace_index[3000]={0};//use for make brace_do_not_owner in recursive
enum family_group{keyword,single_punc,double_punc,var_type,op_comp,op_arith,op_logic,num_i,num_f,chars,var_name};//group the words in codes:
//keyword {main , if , else , while* , void , null, true , false , return} =0
//single_punc{, , ;} =1
//double_punc{() , {} }=2
//var_type{int , float ,bool } =3
//op_comp {> , < , == ,!= } =4
//op_arith {=,+,-,/,*} =5
//op_logic {&& ,||} =6
//num_i {1,2,3,0,681,52,....} =7
//num_f {0.89 ,9.12,....} =8
//chars{'c','a',....} =9
//var_name {mehi ,....}=10 --> error words also save here
//-------------------------------------------------------------------------------------------------------
bool is_integer(string check)//check if the string is an integer or not---//written by UPC
{
    if(check.size()!=1)
    {
        if(check[0]<49 || check[0]>57)//check the first index if it is 0 or not
        {
            return false;
        }
    }
    for(int i=1;i<check.size();i++)//check the other indexes by ASCII code
    {
        if(check[i]<48 || check[i]>57)
        {
            return false;
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------------------------
bool is_float(string check)//check if the string is a float or not---//written by UPC
{
    int dot_position=check.find(".");//fine the dot position
    if(dot_position==string::npos)//check if it has dot or not 12.987606
    {
        return false;
    }
    else if(dot_position==0)//check if its dot is in the first or not
    {
        return false;
    }
    string part1=check.substr(0,dot_position);//separate the digits before dot
    string part2=check.substr(dot_position+1);//separate the digits after dot
    if(!(is_integer(part1)))
    {
        return false;
    }
    for(int i=0;i<part2.size();i++)
    {
        if(part2[i]<48 || part2[i]>57)//check the other indexes by ASCII code
        {
            return false;
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------------------------
int to_intiger(string use)//written by UPC
{
    int change=0;
    int temp_num=0;
    for(int i=0;i<use.size();i++)
    {
        temp_num=use[i];
        change=(change*10)+(temp_num-48);//0 ASCII code is 48
    }
    return change;
}
//-------------------------------------------------------------------------------------------------------
float to_float(string use)//written by UPC
{
    float change_first=0;
    int dot_position=use.find(".");
    int temp_num=0;
    for(int i=0;i<dot_position;i++)
    {
        temp_num=use[i];
        change_first=(change_first*10)+(temp_num-48);//0 ASCII code is 48
    }
    for(int i=dot_position+1;i<use.size();i++)//instant of using continue
    {
        temp_num=use[i];
        change_first=(change_first*10)+(temp_num-48);//0 ASCII code is 48
    }
    int ten=1;
    for(int i=1;i<use.size()-dot_position;i++)//find out how many digits are after the dot
    {
        ten*=10;
    }
    float change=change_first/ten;
    return change;
}
//-------------------------------------------------------------------------------------------------------
bool is_valid(string check)//written by UPC
{
    if(!( (check[0]>64 && check[0]<91) || (check[0]>96 && check[0]<123) ))//check if the first index is alpha
    {
        return false;
    }
    for(int i=1;i<check.size();i++)//check if other indexes is alpha or number
    {
        if(!( (check[i]>47 && check[i]<58) || (check[i]>64 && check[i]<91) || (check[i]>96 && check[i]<123) ))
        {
            return false;
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------------------------
struct tokens//the link list which save all of words except the white words(space,tab,enter)
{
    int word_index;//save the word number use in syntax checking
    family_group family;//save the group as in enum
    string atr;//save the exact word --> caution : we do not save the (') for char type
    int line_number;//save the line number use in error handling and IR Code
    bool has_error;//if the word has an a error it must be true
    bool has_checked;//when the word check we set it to true
    tokens *next;//save the next struct address
} *tok_head=new tokens;//the head of link list
//-------------------------------------------------------------------------------------------------------
struct variable_name//the link list for symbol table
{
    int index;//save the index of struct
    bool unkhown;//if a variable do not have a value it must be true
    string name;//save the variable name as atr string in tokens
    string type;//int , char , bool ,float
    string value;//12 true c 89.023
    variable_name *next;//save the next struct address
} *var_head=new variable_name;//the head of link list
//-------------------------------------------------------------------------------------------------------
void UPC_recursive_check(int start ,const int counter)
{
    if(brace_temp[start+1]==-1)
    {
        int backed=start;
        for(int i=start;i>0;i--)
        {
            if( brace_temp[i]!=-2 && brace_temp[i]!=-1)
            {
                backed--;
            }
        }
        if(brace_temp[backed]==-2 || brace_temp[backed]==-1)
        {
            brace_temp[backed]=brace_index[start+1];
            brace_temp[start+1]=brace_index[backed];
        }
    }
    if(start==counter)
    {
        return;
    }
    UPC_recursive_check(start+1,counter);
}
//-------------------------------------------------------------------------------------------------------
void UPC_set_brace_array()
{
    tokens *tok_temp=tok_head;
    int counter=0;
    while(tok_temp!=NULL)
    {
        if(tok_temp->family==double_punc)
        {
            if(tok_temp->atr=="{")
            {
                brace_temp[counter]=-2;
                brace_index[counter]=tok_temp->word_index;
                tok_temp->has_checked=true;
                counter++;
            }
            else if(tok_temp->atr=="}")
            {
                brace_temp[counter]=-1;
                brace_index[counter]=tok_temp->word_index;
                tok_temp->has_checked=true;
                counter++;
            }
        }
        tok_temp=tok_temp->next;
    }
    UPC_recursive_check(0,counter);
    int k=0;
    counter++;
    for(int i=0;i<counter;i++)
    {
        if(brace_temp[i]==-2 || brace_temp[i]==-1)
        {
            brace_do_not_owner[k]=brace_index[i];
            k++;
        }
    }
}
//-------------------------------------------------------------------------------------------------------
bool UPC_is_closed(int must_be_check)
{
    for(int i=0;brace_do_not_owner[i]!=0;i++)
    {
        if(must_be_check==brace_do_not_owner[i])
        {
            return false;//it does not true because the array save the false index
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------------------------
void UPC_delete_all_after_void(int must_be_delete_index)
{
    tokens *tok_temp=tok_head;
    must_be_delete_index++;
    if(tok_head->word_index==must_be_delete_index)
    {
        cerr<<"your program is wrong and it is not a c program , first learn how to code ; line :: "<<tok_head->line_number<<endl;
        exit(0);
    }
    must_be_delete_index--;
    while(tok_temp->word_index!=must_be_delete_index)
    {
        //UPC :-)
        tok_temp=tok_temp->next;
    }
    tok_temp->next=NULL;
}
//-------------------------------------------------------------------------------------------------------
void UPC_check_the_main()//must be complete
{
    tokens *tok_temp=tok_head;
    int may_be_delete_index=0;
    while(tok_temp->next!=NULL)
    {
        if(tok_temp->family==keyword)
        {
            if(tok_temp->atr=="void")//contain void
            {
                may_be_delete_index=tok_temp->word_index;
                tok_temp->has_checked=true;
                tok_temp=tok_temp->next;//go to next token
                //if--exit
                if(tok_temp==NULL)
                {
                    cerr<<"you program end in here after void !!!! "<<endl;
                    UPC_delete_all_after_void(may_be_delete_index-1);
                    return;
                }
                if(tok_temp->atr=="main")//contain main
                {
                    tok_temp->has_checked=true;
                    tok_temp=tok_temp->next;
                    //if--exit
                    if(tok_temp==NULL)
                    {
                        cerr<<"you program end in here after main !!!! "<<endl;
                        UPC_delete_all_after_void(may_be_delete_index-1);
                        return;
                    }
                    if(tok_temp->atr=="(")
                    {
                        tok_temp->has_checked=true;
                        tok_temp=tok_temp->next;
                        //if--exit
                        if(tok_temp==NULL)
                        {
                            cerr<<"you program end in here after < ( >!!!! "<<endl;
                            UPC_delete_all_after_void(may_be_delete_index-1);
                            return;
                        }
                        if(tok_temp->atr==")")
                        {
                            tok_temp->has_checked=true;
                            tok_temp=tok_temp->next;
                            //if--exit
                            if(tok_temp==NULL)
                            {
                                cerr<<"you program end in here after < ) >!!!! "<<endl;
                                UPC_delete_all_after_void(may_be_delete_index-1);
                                return;
                            }
                            if(tok_temp->atr=="{")
                            {
                                tok_temp->has_checked=true;
                                if(UPC_is_closed(tok_temp->word_index))
                                {
                                    //make the other code after last } error and check ---cerr
                                    for(int i=0;brace_index[i]!=0;i)
                                    {
                                        if(brace_index[i]==tok_temp->word_index)
                                        {
                                            may_be_delete_index=brace_temp[i];
                                            tok_temp=tok_temp->next;
                                            while(tok_temp->word_index!=may_be_delete_index)
                                            {
                                                  tok_temp=tok_temp->next;
                                            }
                                            tok_temp->has_checked=true;
                                            if(tok_temp->next!=NULL)
                                            {
                                                  tok_temp->next=NULL;
                                                  delete tok_temp->next;
                                            }
                                            return;
                                        }
                                    }
                                }
                                else
                                {
                                    //write later make error and check
                                    cerr<<"your program does not contain < } > last line"<<endl;
                                    UPC_delete_all_after_void(may_be_delete_index-1);
                                    return;
                                }
                            }
                            else
                            {
                                //write later make error and check
                                cerr<<"your program does not contain < { > line :: "<<tok_temp->line_number<<endl;
                                UPC_delete_all_after_void(may_be_delete_index-1);
                                return;
                            }
                        }
                        else
                        {
                            //write later make error and check
                            cerr<<"your program does not contain < ) > line :: "<<tok_temp->line_number<<endl;
                            UPC_delete_all_after_void(may_be_delete_index-1);
                            return;
                        }
                    }
                    else
                    {
                        //write later make error and check
                        cerr<<"your program does not contain < ( > line :: "<<tok_temp->line_number<<endl;
                        UPC_delete_all_after_void(may_be_delete_index-1);
                        return;
                    }
                }
                else
                {
                    //write later make error and check
                    cerr<<"your program does not contain main line :: "<<tok_temp->line_number<<endl;
                    UPC_delete_all_after_void(may_be_delete_index-1);
                    return;
                }
            }
        }
        tok_temp=tok_temp->next;
    }
}
//-------------------------------------------------------------------------------------------------------
int UPC_make_it_error_and_checked(int which_index)//make all tokens between two ; error and check--->caution : can not use for main ,if , else , while
{
    int start=0,finish=0;
    tokens *tok_temp=tok_head;
    while(tok_temp->word_index!=which_index)
    {
        if(tok_temp->atr==";")
        {
            start=tok_temp->word_index;
        }
        else if(tok_temp->atr=="}")
        {
            start=tok_temp->word_index;
        }
        else if(tok_temp->atr=="{")
        {
            start=tok_temp->word_index;
        }
        tok_temp=tok_temp->next;
    }
    while(tok_temp->next!=NULL)
    {
        if(tok_temp->atr==";")
        {
            finish=tok_temp->word_index;
            break;
        }
        else if(tok_temp->atr=="}")
        {
            finish=tok_temp->word_index;
            finish--;
            break;
        }
        else if(tok_temp->atr=="{")
        {
            finish=tok_temp->word_index;
            finish--;
            break;
        }
        tok_temp=tok_temp->next;
    }
    start++;
    tok_temp=tok_head;
    while(tok_temp->next!=NULL)
    {
        if(tok_temp->word_index>=start && tok_temp->word_index<=finish)
        {
            tok_temp->has_checked=true;
            tok_temp->has_error=true;
        }
        tok_temp=tok_temp->next;
    }
    return (finish+1);
}
//-------------------------------------------------------------------------------------------------------
void UPC_delete_all_between_if_and_closed_brace(int start_delete,int finish_delete)
{
    start_delete--;
    tokens *tok_start_temp=tok_head;
    while(tok_start_temp->word_index!=start_delete)
    {
        //UPC :-)
        tok_start_temp=tok_start_temp->next;
    }
    tokens *tok_finish_temp=tok_start_temp;
    while(tok_finish_temp->word_index!=finish_delete)
    {
        //UPC :-)
        tok_finish_temp=tok_finish_temp->next;
    }
    tok_start_temp=tok_finish_temp->next;
}
//-------------------------------------------------------------------------------------------------------
void UPC_check_the_if_else()
{
    tokens *tok_temp=tok_head;
    int may_be_delete_index=0;
    while(tok_temp->next!=NULL)
    {
        if(tok_temp->family==keyword)
        {
            if(tok_temp->atr =="if")
            {
                may_be_delete_index=tok_temp->word_index;
                tok_temp->has_checked=true;
                tok_temp=tok_temp->next;//go to next token
                if(tok_temp->atr=="(")
                {
                    //var_name op_... var_name )
                    //call a function for check the if statement has one start token *
                }
                else
                {
                    //check for  ) or { then delete it up to ) or }
                    while(tok_temp->atr!=";")
                    {
                        if(tok_temp->atr=="{")
                        {
                            int last_index_to_delete=0;
                            for(int i=0;brace_index[i]!=0;i++)
                            {
                                if(brace_index[i]==tok_temp->word_index);
                            }
                            return;
                        }
                        tok_temp=tok_temp->next;
                    }
                }
            }
            else if(tok_temp->atr=="else")
            {
                //check {
            }
        }
        tok_temp=tok_temp->next;
    }
}
//-------------------------------------------------------------------------------------------------------
void UPC_pre_process()
{
    //-----the variable we need for check and find main function and include file
    int void_position;
    int main_position;
    int sharp_position;//#
    int include_position;
    int file_name_position;
    int include_1_position;//<
    int include_2_position;//>
    bool void_existance = false;
    bool main_existance = false;
    bool sharp_existance = false;
    bool include_existance = false;
    bool include_1_existance = false;
    bool include_2_existance = false;
    string user_main;
    string user_include;
    string include_file_name;
    //------read main.c and save it in string
    ifstream read_main ("main.c",ios::in);
    if(!read_main)
    {
        cerr<<"ERROR in file opening ---> check your main.c"<<endl;
        exit(0);
    }
    if(read_main.peek()== std::ifstream::traits_type::eof())//empty main
    {
        cerr<<"nothing to do !!!! ---> empty file for compile"<<endl;
        exit(0);
    }
    while(true)
    {
        string temp;
        if(read_main.eof())
        {
            break;
        }
        getline(read_main,temp);
        user_main = user_main + temp;
        user_main += "\n";
    }
    cout<<"reading main.c ...."<<endl;
    read_main.close();
    //----find what we need and error handling
    main_position = user_main.find("main");
    if(main_position!=string::npos)
    {
        main_existance=true;
        void_position = user_main.find("void");
        if(void_position!=string::npos)
        {
            void_existance=true;
        }
        else
        {
            cerr<<"error : You do not define the main type"<<endl;
        }
    }
    else
    {
        cerr<<"error : Your program does not contain main"<<endl;
    }
    sharp_position = user_main.find("#");
    cout<<sharp_position<<endl;
    if(sharp_position!=string::npos)
    {
       sharp_existance=true;
       include_position = user_main.find("include");
       if(include_position!=string::npos)
       {
            include_existance=true;
            include_1_position= user_main.find("<");
            if(include_1_position!=string::npos)
            {
                include_1_existance=true;
                include_2_position= user_main.find(">");
                if(include_2_position!=string::npos)
                {
                    include_2_existance=true;
                    /*if(include_position-sharp_position!=2 || include_1_position-include_position!=8 || (include_2_position-include_1_position>=20&& include_2_position-include_1_position<2))
                    {
                        cerr<<"error : un correct order for include !"<<endl;
                        ofstream go_compile("compile.c",ios::out);
                        if(!go_compile)
                        {
                            cerr<<"ERROR in create a new file for compile "<<endl;
                        }
                        go_compile<<user_main;
                        cout<<"writing codes in new file ...."<<endl;
                        go_compile.close();
                        return;
                    }*/
                }
                else
                {
                    cerr<<"error : You do not use the open include (>) for header name"<<endl;
                    ofstream go_compile("compile.c",ios::out);
                    if(!go_compile)
                    {
                        cerr<<"ERROR in create a new file for compile "<<endl;
                    }
                    go_compile<<user_main;
                    cout<<"writing codes in new file ...."<<endl;
                    go_compile.close();
                    return;
                }
            }
            else
            {
                cerr<<"error : You do not use the open include (<) for header name"<<endl;
                ofstream go_compile("compile.c",ios::out);
                if(!go_compile)
                {
                    cerr<<"ERROR in create a new file for compile "<<endl;
                }
                go_compile<<user_main;
                cout<<"writing codes in new file ...."<<endl;
                go_compile.close();
                return;
            }
        }
        else
        {
            cerr<<"error : You dont use the include after your pre process (#) "<<endl;
            ofstream go_compile("compile.c",ios::out);
            if(!go_compile)
            {
                cerr<<"ERROR in create a new file for compile "<<endl;
            }
            go_compile<<user_main<<endl;
            cout<<"writing codes in new file ...."<<endl;
            go_compile.close();
            return;
        }
    }
    else
    {
        ofstream go_compile("compile.c",ios::out);
        if(!go_compile)
        {
            cerr<<"ERROR in create a new file for compile "<<endl;
        }
        go_compile<<user_main<<endl;
        cout<<"writing codes in new file ...."<<endl;
        go_compile.close();
        return;
    }
    //------writing together
    if(sharp_existance)
    {
        if(include_existance)
        {
            include_file_name=user_main.substr(include_1_position+2,include_2_position-include_1_position-3);
            char temp_name[ include_file_name.size()];
            for(int i=0;i< include_file_name.size();i++)
            {
                temp_name[i]= include_file_name[i];
            }
            ifstream read_include(temp_name,ios::in);
            if(!read_include)
            {
                cerr<<"ERROR in file opening check your including file "<<endl;
                return;
            }
            if(read_include.peek()== std::ifstream::traits_type::eof())//empty include
            {
                cout<<"nothing to do !!!! ---> empty included file for compile"<<endl;
                ofstream go_compile("compile.c",ios::out);
                if(!go_compile)
                {
                    cerr<<"ERROR in create a new file for compile "<<endl;
                }
                go_compile<<user_main;
                cout<<"writing codes in new file ...."<<endl;
                return;

            }
            while(true)
            {
                string temp;
                getline(read_include,temp);
                user_include = user_include + temp;
                user_include += "\n";
                if(read_include.eof())
                {
                    break;
                }
            }
            cout<<"reading include file ...."<<endl;
            user_include.erase(user_include.size()-2,2);
            user_main.erase(sharp_position,include_2_position-sharp_position+1);
            user_main.erase(user_main.size()-2,2);
            read_include.close();
            //-------------------
            ofstream go_compile("compile.c",ios::out);
            if(!go_compile)
            {
                cerr<<"ERROR in create a new file for compile "<<endl;
            }
            go_compile<<user_include<<user_main;
            cout<<"writing codes in new file ...."<<endl;
            go_compile.close();
        }
    }
}
//-------------------------------------------------------------------------------------------------------
void UPC_full_tokens_manage_first_steps()
{
    ifstream compile("compile.c",ios::in);
    if(!compile )
    {
        cerr<<"ERROR : can not open the compile file !"<<endl;
        return;
    }
    string line;
    string word;
    int line_counter=0;
    int word_counter=0;
    tokens *tok_temp=tok_head;
    while(true)
    {
        getline(compile,line);
        line_counter++;
        if(!line.empty())
        {
            ofstream file_temp_write("temp.SM&UPC",ios::out);
            if(!file_temp_write )
            {
                cerr<<"ERROR : can not open the temp file !"<<endl;
                return;
            }
            file_temp_write<<line;
            file_temp_write.close();
            ifstream file_temp_read("temp.SM&UPC",ios::in);
            if(!file_temp_read )
            {
                cerr<<"ERROR : can not open the temp file !"<<endl;
                return;
            }
            while(true)
            {
                file_temp_read>>word;
                word_counter++;
                //---------checking keyword
                if(word=="main")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="if")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="else")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="void")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="null")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="true")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="false")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="return")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }

                else if(word=="while")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=keyword;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking single_punc
                else if(word==",")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=single_punc;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word==";")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=single_punc;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking double_punc
                else if(word=="(")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=double_punc;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word==")")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=double_punc;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="{")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=double_punc;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="}")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=double_punc;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking var_type
                else if(word=="int")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=var_type;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="bool")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=var_type;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="char")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=var_type;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="float")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=var_type;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking op_comp
                else if(word==">")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_comp;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="<")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_comp;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="==")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_comp;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="!=")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_comp;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking op_arith
                else if(word=="=")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_arith;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="+")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_arith;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="-")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_arith;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="*")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_arith;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="/")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_arith;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking op_logic
                else if(word=="&&")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_logic;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                else if(word=="||")
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=op_logic;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking chars
                else if(word.find("'")!=string::npos)
                {
                    int quote_first=word.find("'");
                    if(quote_first==0)
                    {
                        int quote_second=word.find("'",quote_first+1);
                        if(quote_second!=string::npos)
                        {
                            if(quote_second-quote_first==2)
                            {
                                tok_temp->next=NULL;
                                tok_temp->atr=word.substr(1,1);
                                tok_temp->family=chars;
                                tok_temp->line_number=line_counter;
                                tok_temp->has_error=false;
                                tok_temp->has_checked=false;
                                tok_temp->word_index=word_counter;
                            }
                            else
                            {
                                cerr<<"an un known word in line :: "<<line_counter<<endl;
                                tok_temp->next=NULL;
                                tok_temp->atr=word;
                                tok_temp->family=var_name;
                                tok_temp->line_number=line_counter;
                                tok_temp->has_error=true;
                                tok_temp->has_checked=false;
                                tok_temp->word_index=word_counter;
                            }
                        }
                        else
                        {
                            cerr<<"an un known word in line :: "<<line_counter<<endl;
                            tok_temp->next=NULL;
                            tok_temp->atr=word;
                            tok_temp->family=var_name;
                            tok_temp->line_number=line_counter;
                            tok_temp->has_error=true;
                            tok_temp->has_checked=false;
                            tok_temp->word_index=word_counter;
                        }
                    }
                    else
                    {
                        cerr<<"an un known word in line :: "<<line_counter<<endl;
                        tok_temp->next=NULL;
                        tok_temp->atr=word;
                        tok_temp->family=var_name;
                        tok_temp->line_number=line_counter;
                        tok_temp->has_error=true;
                        tok_temp->has_checked=false;
                        tok_temp->word_index=word_counter;
                    }
                }
                //---------checking num_i
                else if(is_integer(word))
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=num_i;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking num_f
                else if(is_float(word))
                {
                    tok_temp->next=NULL;
                    tok_temp->atr=word;
                    tok_temp->family=num_f;
                    tok_temp->line_number=line_counter;
                    tok_temp->has_error=false;
                    tok_temp->has_checked=false;
                    tok_temp->word_index=word_counter;
                }
                //---------checking var_name
                else
                {
                    if(is_valid(word))
                    {
                        tok_temp->next=NULL;
                        tok_temp->atr=word;
                        tok_temp->family=var_name;
                        tok_temp->line_number=line_counter;
                        tok_temp->has_error=false;
                        tok_temp->has_checked=false;
                        tok_temp->word_index=word_counter;
                    }
                    else
                    {
                        cerr<<"an un known word in line :: "<<line_counter<<endl;
                        tok_temp->next=NULL;
                        tok_temp->atr=word;
                        tok_temp->family=var_name;
                        tok_temp->line_number=line_counter;
                        tok_temp->has_error=true;
                        tok_temp->has_checked=false;
                        tok_temp->word_index=word_counter;
                    }
                }
                tok_temp->next=new tokens;
                tok_temp=tok_temp->next;
                //***********************************************
                if(file_temp_read.eof())
                {
                    cout<<"reading line : "<<line_counter<<endl;
                    break;
                }
            }
        }
        if(compile.eof())
        {
            cout<<"The file tokening done successfully :-)"<<endl;
            break;
        }
    }
    tok_temp->next=NULL;
    UPC_set_brace_array();
    UPC_check_the_main();
    tok_temp=tok_head;
    int next_check=1;
    while(tok_temp->next!=NULL)
    {
        if(tok_temp->has_error==true && tok_temp->word_index>=next_check)
        {
            next_check=UPC_make_it_error_and_checked(tok_temp->word_index);
        }
        tok_temp=tok_temp->next;
    }
    compile.close();
}
//-------------------------------------------------------------------------------------------------------
void UPC_make_compile_log()
{
    ofstream log("UPC.log",ios::out);
    if(!log)
    {
        cerr<<"can not open the log file !!!"<<endl;
    }
    tokens *t=tok_head;
    while(t!=NULL)
    {
        log<<"the atr is : "<<t->atr<<endl;
        log<<"the family is : "<<t->family<<endl;
        log<<"the line number is : "<<t->line_number<<endl;
        log<<"the word number is : "<<t->word_index<<endl;
        log<<"the error is : "<<t->has_error<<endl;
        log<<"the checked is : "<<t->has_checked<<endl;
        log<<"----------------------------------------"<<endl;
        t=t->next;
    }
    log<<"********now the brace array********"<<endl;
    for(int i=0;brace_do_not_owner[i]!=0;i++)
    {
        log<<brace_do_not_owner[i]<<"\t";
    }
    log<<endl<<"********now the brace array temp********"<<endl;
    for(int i=0;brace_temp[i]!=0;i++)
    {
        log<<brace_temp[i]<<"\t";
    }
    log<<endl<<"********now the brace array index********"<<endl;
    for(int i=0;brace_index[i]!=0;i++)
    {
        log<<brace_index[i]<<"\t";
    }
}
//-------------------------------------------------------------------------------------------------------
int main()
{
    UPC_pre_process();
    UPC_full_tokens_manage_first_steps();
    UPC_make_compile_log();
    return 0;
}
