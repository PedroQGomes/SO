# MIEI - 2ºAno - Sistemas Operativos

O trabalho pratico proposto consiste no desenvolvimento de um sistema de gestão de inventários e vendas. O sistema é constituído por vários programas: manutenção de artigos, servidor de vendas, cliente de vendas e agregador de dados. O trabalho foi desenvolvido na linguagem **C**, tendo em mente a utilização de sistem calls presentes em **Linux** abordadas durante a **UC**.

* **(ma)** Manutenção de Artigos é responsável pela inserção de novos artigos assim como a mudança de preço/nome de artigos existentes.
* **(sv)** Servidor de Vendas é responsável pelo controlo de stock e pelo registo de vendas/compras efetuadas.
* **(cv)** Cliente de Vendas faz pedidos ao Servidor de Vendas, consulta de stock, entrada de stock entre outros.
* **(ag)** Agregador de Dados é um programa que é chamado pelo Servidor de Vendas para comprimir o ficheiro de vendas.


# Compilação e Testes

Para compilar todos os programas é apenas necessário correr o seguinte comando

```
make
```

Para executar o programa de **Manutenção de Artigos** é usado o seguinte comando
```
./ma
```

Para executar o programa **Servidor de Vendas** é usado o seguinte comando
```
./sv
```

Para executar o programa **Cliente de Vendas** é usado o seguinte comando
```
./cv
```

Para executar o programa **Agregador de Dados** é usado o seguinte comando
```
./ag
```

Para limpar a diretoria é usado o seguinte comando

```
make clean
```