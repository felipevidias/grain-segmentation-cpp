# ====================================================================
# 1. Definição do Projeto e Padrão do C++
# ====================================================================

# Versão mínima do CMake necessária para rodar este script
cmake_minimum_required(VERSION 3.16)

# Nome do projeto e linguagem principal (C++)
project(GrainAnalysisProject CXX)

# Define o padrão C++ para C++17, o mesmo que temos usado
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ====================================================================
# 2. Encontrando as Dependências (Higra, TIFF, Threads)
# ====================================================================
# O CMake irá procurar por estas bibliotecas no seu sistema.
# Ele procura em locais padrão como /usr/local/ e /usr/

# Encontra a biblioteca de Threads, necessária para o dstyle.cpp
find_package(Threads REQUIRED)

# Encontra a biblioteca TIFF, necessária para carregar as imagens
find_package(TIFF REQUIRED)

# Encontra a biblioteca Higra, que você instalou.
# Ele procura por um arquivo de configuração que a Higra instala.
find_package(higra REQUIRED)

# Para a xtensor, que é header-only, vamos apenas adicionar o caminho
# para os seus includes mais tarde.


# ====================================================================
# 3. Adicionando o Executável e Listando os Arquivos Fonte
# ====================================================================
# Aqui nós definimos o nome do nosso programa final e listamos
# TODOS os arquivos .cpp que precisam ser compilados.

add_executable(meu_programa
    main.cpp
    src/contact_points/contact_detection/common.cpp
    src/contact_points/contact_detection/contact_detection_by_extending_labels.cpp
    src/contact_points/contact_detection/contact_detection_from_label_and_skeleton.cpp
    src/contact_points/contact_detection/contact_detection_naive.cpp
    src/utils/dstyle.cpp
    src/utils/getCentroid.cpp
    src/utils/minTree.cpp
)


# ====================================================================
# 4. Adicionando os Caminhos de Inclusão (Headers)
# ====================================================================
# Dizemos ao nosso programa onde encontrar os arquivos .hpp.

target_include_directories(meu_programa PRIVATE
    # O caminho para a pasta include da xtensor que você baixou
    /home/felipe/dev/xtensor/include # <-- ATENÇÃO: VERIFIQUE SE ESTE CAMINHO ESTÁ CORRETO!

    # Os cabeçalhos do nosso próprio projeto
    ${CMAKE_CURRENT_SOURCE_DIR}/src/contact_points/contact_detection/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/include
)


# ====================================================================
# 5. Linkando as Bibliotecas
# ====================================================================
# Esta é a etapa final, onde conectamos nosso programa com as
# bibliotecas que o CMake encontrou.

target_link_libraries(meu_programa PRIVATE
    # O :: é a sintaxe moderna do CMake para linkar bibliotecas
    Threads::Threads
    TIFF::TIFF
    higra::higra
)

# Mensagem final para o usuário
message(STATUS "Configuração do CMake concluída. Use 'make' para compilar.")