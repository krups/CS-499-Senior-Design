/**
 * @brief Class for sensor data 
 * 
 */
class Data
{
public:
    Data(char *line);
    int getType();
    int getNumVals();
    u_int16_t getTimeStamp();
    u_int16_t *getData();
    ~Data();

private:
    int type;
    int num_vals;
    u_int16_t time_stamp;
    u_int16_t *data;
};