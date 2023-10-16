// #include <iomanip>
// #include <stdlib.h>
// #include <optional>

// #include "internalRep.h"
// #include "compilerUtils.h"

// // Framework
// // These functions are provided for you, and do not need modified for project 0.
// // They will also serve as a template for project 1.
// string showRelType(RelType rel) {
//   switch(rel) {
//     case SUM:
//       return "sum";
//       break;
//     case PRODUCT:
//       return "product";
//       break;
//     case NEITHER:
//       return "neither";
//       break;
//     default:
//       error("Improperly initialized RelType"); 
//       return "";
//       break;
//   }
// }

// string Relation::showLocation() {
//   switch(location) {
//     case 0:
//       return "0th";
//       break;
//     case 1:
//       return "1st";
//       break;
//     case 2:
//       return "2nd";
//       break;
//     default:
//       error("Improperly initialized location.");
//       return "";
//       break;
//   }
// }

// void Relation::prettyPrint() {
//   switch(cat) {
//     case SUM:
//     case PRODUCT:
//       cout << "//The " << showLocation() << " location is the " << showRelType(cat) << " of the other two." << endl;
//       break;
//   }
//   return;
// }

// void Relation::tablePrint() {
//   cout << left << setw(7) << showRelType(cat) << " | " << setw(4) << location;
//   return;
// }

// void Line::prettyPrint() {
//   cout << numZero << " " << numOne << " " << numTwo << endl;
//   rel.prettyPrint();
//   return;
// }

// void Line::tablePrint() {
//   cout << "//|| "; 
//   rel.tablePrint();
//   cout << " || " << left << setw(4) << numZero << " | " << setw(4) << numOne 
//     << setw(4) << " | " << setw(4) << numTwo << " ||" << endl;
//   return;
// }

// int readInt(ifstream &input) {
//   char c;
//   int res{ 0 };
//   skipBlanks(input);
//   input.get(c);
//   while (input && isNum(c)) {
//       res *= 10;
//       res += (c - '0');
//       input.get(c);
//   }
//   debug("Final integer from readInt: " + to_string(res)); // use the -v 1 flag to test this
//   return res;
// }

// bool isNum(char c) {
//     return '0' <= c && c <= '9';
// }

// Line readLine(ifstream &input, bool &valid) {
//   char c;
//   Line l;
//   skipWhitespace(input);
//   if(input.good() && input.peek() == '/') {
//     input.get(c);
//     ignoreComment(input);
//     return readLine(input, valid);
//   } else {
//     if(!input.good()) {
//       valid = false;
//       return l;
//     }
//     l.numZero = readInt(input);
//     l.numOne = readInt(input);
//     l.numTwo = readInt(input);
//     l.rel = getRelation(l);
//     return l;
//   }
// }

// Relation getRelation(Line l) {
//     int nums[] { l.numZero, l.numOne, l.numTwo };
//     Relation rel;

//     for (int i = 0; i < 3; ++i)
//     {
//         int a{ nums[(i + 1) % 3] };
//         int b{ nums[(i + 2) % 3] };
//         if (a + b == nums[i])
//         {
//             rel.cat = RelType::SUM;
//             rel.location = i;
//             return rel;
//         }
//         if (a * b == nums[i])
//         {
//             rel.cat = RelType::PRODUCT;
//             rel.location = i;
//             return rel;
//         }
//     }
//     rel.cat = RelType::NEITHER;
//     return rel;
// }

// vector<Line> readFile(ifstream &input, vector<int> &summary) {
//   bool valid = true;
//   vector<Line> lines;
//   Line lineOpt;
//   lineOpt = readLine(input, valid);
//   while(valid) {
//     lines.push_back(lineOpt);
//     lineOpt = readLine(input,valid);
//   }
//   createSummary(lines, summary);
//   return(lines);
// }

// void createSummary(const vector<Line> &lines, vector<int> &summary) 
// {
//     for (const Line& line : lines) {
//         ++summary[line.numZero];
//         ++summary[line.numOne];
//         ++summary[line.numTwo];
//     }
// }
