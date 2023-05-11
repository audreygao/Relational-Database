 //
//  DumpView.cpp
//  assignment2
//
//  Created by Ya Gao
//

#include "DumpView.hpp"

namespace ECE141 {

  DumpView::DumpView(std::vector<char> types, bool success, double aTime): types(types), success(success), time(aTime){}

  bool DumpView::show(std::ostream &aStream){
    if(success) {
        std::string rowCase = (types.size() == 1) ? " row" : " rows";
        //int index = 0;

        aStream << "+------------------------+\n";
        aStream << "|          Type          |\n";
        aStream << "+------------------------+\n";

        for(auto block: types) {
            std::string type;
            switch (block) {
                case 'M': 
                    type = "Meta";
                    break;
                case 'D': type = "Data";
                    break;
                case 'E': type = "Entity";
                    break;
                case 'F': type = "Free";
                    break;
                case 'I': type = "Index";
                    break;
                default: break;
            }
        aStream << "| " << std::left << std::setw(22) << type << " |\n";
        aStream << "+------------------------+\n";

        }
        aStream << types.size() << rowCase << " in set " << "(" << std::setprecision(2) << time << " sec)\n";
        return true;
    }
    return false;
  }

}
