#include "TableView.hpp"
#include "Row.hpp"

namespace ECE141 {

    TableView::TableView(RowCollection &aRows, Query& aQuery, double aTime) : 
      rows(std::move(aRows)), query(aQuery), time(aTime) {
      }

    bool TableView::show(std::ostream &anOutput) {

      int limit = rows.size();
      
      if(query.getLimit() > 0 && query.getLimit() <= rows.size()) limit = query.getLimit();
      
      //sort the rows with orderby keyword
      if(query.getOrderField() != "") {
        std::sort(rows.begin(), rows.end(), [&](std::unique_ptr<Row>& row1, std::unique_ptr<Row>& row2){
          return row1 -> getValue(query.getOrderField()) < row2 -> getValue(query.getOrderField());
        });
      }

      //set the fields to display
      StringList fields;
      if(query.selectAll()) {
          AttributeList attributes;
          query.getFrom()->getAttributeList(attributes);
          fields = query.getFrom()->getAttributeNamesList(attributes);
      } else {
          fields = query.getSelect();
      }
      int size = fields.size();

      printDivider(fields, anOutput);
      anOutput << "| ";
      for(int i = 0; i < size; i++) {
          anOutput << std::setw(22) << fields.at(i) << " | ";
      }
      anOutput << "\n";
      printDivider(fields, anOutput);

      int count = 0;
      for(auto && row: rows) {
          if(count >= limit) break;
          for(int i = 0; i < size; i++) {
            std::stringstream str;
            if( !row->hasKey(fields.at(i)) )
              str << "NULL";
            else
              str << row->getValue(fields.at(i));
            anOutput << "| " << std::setw(22) << str.str() << " ";
          }
        anOutput << "|\n";
        printDivider(fields, anOutput);
          count++;
      }

      std::string rowCase = (rows.size() == 1) ? " row" : " rows";
      anOutput << limit << rowCase << " in set " << "(" << std::setprecision(2) << time << " sec)\n";
      return true;
    }
}

