#include "common.hpp"

std::ostream& operator<<(std::ostream& cout, const glm::mat4& m){
    cout << std::setw(6) << '[';
    for (int i = 0; i < 4; i++){
        if (i)
            cout << ' ';
        cout << m[i].x << ", " << m[i].y << ", " << m[i].z << ", " << m[i].w;
        if (i != 3)
            cout << std::endl << std::setw(6);
    }
    cout << ']' << std::endl;
    return cout;
}

std::ostream& operator<<(std::ostream& cout, const glm::vec4& m){
    cout << '[' << m.x << ", " << m.y << ", " << m.z << ", " << m.w << ']' << std::endl;
    return cout;
}
