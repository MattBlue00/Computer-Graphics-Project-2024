#ifndef DEBUG_HPP
#define DEBUG_HPP

void DEBUG_printMAT4(const glm::mat4& matrix) {
    std::cout << std::fixed << std::setprecision(3);

    for (int row = 0; row < 4; ++row) {
        std::cout << "[ ";
        for (int col = 0; col < 4; ++col) {
            // Accesso alla riga `row` e colonna `col`
            std::cout << matrix[col][row] << " ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;
}

#endif
