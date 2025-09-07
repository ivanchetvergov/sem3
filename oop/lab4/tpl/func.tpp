// func.tpp

template <typename Container>
void printContainer(const Container& cont) {
    for (const auto& item : cont) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

template <typename T>
void printPop(T container) {
    while (!container.empty()) {
        std::cout << container.top() << " "; // front() for queue
        container.pop();
    }
    std::cout << std::endl;
} 

template <typename T>
void printMap(const T& container) {
    for (const auto& pair : container) {
        std::cout << "{" << pair.first << ": " << pair.second << "} ";
    }
    std::cout << std::endl;
}

template<typename T>
void print(const T& item) {
    std::cout << item << " ";
}

