#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>

// ------------------------------------------------------
// 1. Validation: check if a string is a valid double
// ------------------------------------------------------
bool isValidDouble(const std::string& s) {
    if (s.empty()) return false;

    int i = 0;
    if (s[i] == '+' || s[i] == '-') i++;
    bool hasDigitsBefore = false, hasDigitsAfter = false, hasDot = false;

    for (; i < (int)s.size(); ++i) {
        char c = s[i];
        if (isdigit(c)) {
            if (!hasDot) hasDigitsBefore = true;
            else hasDigitsAfter = true;
        } else if (c == '.') {
            if (hasDot) return false; // multiple dots
            hasDot = true;
        } else {
            return false; // invalid character
        }
    }

    if (!hasDigitsBefore) return false;
    if (hasDot && !hasDigitsAfter) return false;
    return true;
}

// ------------------------------------------------------
// 2. Helper: Add two unsigned integer strings
// ------------------------------------------------------
std::string addIntegerStrings(std::string a, std::string b) {
    std::string result = "";
    int carry = 0;
    int i = (int)a.size() - 1, j = (int)b.size() - 1;

    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) sum += a[i--] - '0';
        if (j >= 0) sum += b[j--] - '0';
        carry = sum / 10;
        result.push_back((sum % 10) + '0');
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// ------------------------------------------------------
// 3. Helper: Subtract two unsigned integer strings (a >= b)
// ------------------------------------------------------
std::string subtractIntegerStrings(std::string a, std::string b) {
    std::string result = "";
    int borrow = 0;
    int i = (int)a.size() - 1, j = (int)b.size() - 1;

    while (i >= 0) {
        int diff = (a[i] - '0') - borrow - (j >= 0 ? (b[j] - '0') : 0);
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else borrow = 0;
        result.push_back(diff + '0');
        i--; j--;
    }
    while (result.size() > 1 && result.back() == '0')
        result.pop_back();
    std::reverse(result.begin(), result.end());
    return result;
}

// ------------------------------------------------------
// 4. Strip sign (+/-) and return absolute value
// ------------------------------------------------------
std::string stripSign(const std::string &s, bool &isNegative) {
    std::string num = s;
    isNegative = false;
    if (num[0] == '+') num = num.substr(1);
    else if (num[0] == '-') {
        num = num.substr(1);
        isNegative = true;
    }
    return num;
}

// ------------------------------------------------------
// 5. Compare absolute values (handles decimals correctly)
// ------------------------------------------------------
int compareAbs(const std::string &a, const std::string &b) {
    std::string A = a, B = b;

    // Find decimal positions
    size_t dotA = A.find('.');
    size_t dotB = B.find('.');
    int decA = (dotA == std::string::npos) ? 0 : (A.size() - dotA - 1);
    int decB = (dotB == std::string::npos) ? 0 : (B.size() - dotB - 1);
    int maxDec = std::max(decA, decB);

    // Ensure both have decimals for normalization
    if (dotA == std::string::npos) A += '.';
    if (dotB == std::string::npos) B += '.';
    while ((int)(A.size() - A.find('.') - 1) < maxDec) A += '0';
    while ((int)(B.size() - B.find('.') - 1) < maxDec) B += '0';

    // Remove dots for integer comparison
    A.erase(std::remove(A.begin(), A.end(), '.'), A.end());
    B.erase(std::remove(B.begin(), B.end(), '.'), B.end());

    // Remove leading zeros
    A.erase(0, A.find_first_not_of('0'));
    B.erase(0, B.find_first_not_of('0'));
    if (A.empty()) A = "0";
    if (B.empty()) B = "0";

    if (A.size() != B.size()) return (A.size() > B.size()) ? 1 : -1;
    if (A == B) return 0;
    return (A > B) ? 1 : -1;
}

// ------------------------------------------------------
// 6. Add two doubles as strings (handles sign and decimals)
// ------------------------------------------------------
std::string addStrings(std::string a, std::string b) {
    bool negA = false, negB = false;
    a = stripSign(a, negA);
    b = stripSign(b, negB);

    // Normalize decimal parts
    size_t dotA = a.find('.');
    size_t dotB = b.find('.');
    int decA = (dotA == std::string::npos) ? 0 : (a.size() - dotA - 1);
    int decB = (dotB == std::string::npos) ? 0 : (b.size() - dotB - 1);
    int maxDec = std::max(decA, decB);

    if (dotA == std::string::npos) a += '.';
    if (dotB == std::string::npos) b += '.';
    while ((int)(a.size() - a.find('.') - 1) < maxDec) a += '0';
    while ((int)(b.size() - b.find('.') - 1) < maxDec) b += '0';

    // Remove dots for integer math
    a.erase(std::remove(a.begin(), a.end(), '.'), a.end());
    b.erase(std::remove(b.begin(), b.end(), '.'), b.end());

    std::string result;
    bool resultNegative = false;

    if (negA == negB) {
        // Same sign → addition
        result = addIntegerStrings(a, b);
        resultNegative = negA;
    } else {
        // Different signs → subtraction
        int cmp = compareAbs(a, b);
        if (cmp == 0) {
            result = "0";
            resultNegative = false;
        } else if (cmp > 0) {
            result = subtractIntegerStrings(a, b);
            resultNegative = negA;
        } else {
            result = subtractIntegerStrings(b, a);
            resultNegative = negB;
        }
    }

    // Insert decimal point
    if (maxDec > 0) {
        if (result.size() <= (size_t)maxDec)
            result.insert(0, std::string(maxDec - result.size() + 1, '0'));
        result.insert(result.end() - maxDec, '.');
    }

    // Remove leading zeros
    while (result.size() > 1 && result[0] == '0' && result[1] != '.')
        result.erase(result.begin());

    // Remove trailing zeros after decimal
    if (result.find('.') != std::string::npos) {
        while (result.back() == '0') result.pop_back();
        if (result.back() == '.') result.pop_back();
    }

    if (resultNegative && result != "0") result = "-" + result;
    return result;
}

// ------------------------------------------------------
// 7. Process the input file line by line
// ------------------------------------------------------
void processFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return;
    }

    std::string a, b;
    while (file >> a >> b) {
        std::cout << "Input: " << a << " , " << b << "\n";
        if (!isValidDouble(a) || !isValidDouble(b)) {
            std::cout << "Invalid double format detected.\n\n";
            continue;
        }
        std::string result = addStrings(a, b);
        std::cout << "Sum: " << result << "\n\n";
    }
    file.close();
}

// ------------------------------------------------------
// 8. Main
// ------------------------------------------------------
int main() {
    std::string filename;
    std::cout << "Enter filename: ";
    std::cin >> filename;
    processFile(filename);
    return 0;
}
