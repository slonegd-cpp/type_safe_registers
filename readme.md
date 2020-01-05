Попытаюсь разобраться с безопасной работой с регистрами за счёт строгой типизации с++.

В последней итерации библиотеки, которую я разрабатывал, инициализация dma, к примеру, выглядит так:
```c++
some_stream.set (Direction::to_periph)
    .set_memory_adr (buffer.begin())
    .set_periph_adr (usart.transmit_data_adr())
    .inc_memory()
    .size_memory (DataSize::byte8)
    .size_periph (DataSize::byte8)
    .enable_transfer_complete_interrupt();
```

ООП подход довольно безопасный, но оверхедный, так как каждая строка в примере - отдельное обращение к регистру, хотя несколько параметров можно задать за раз.

Хотелось бы сделать чтото вроде:
```c++
some_stream.set(
      dma_stream::direction::to_periph
    , dma_stream::memory_address {buffer.begin()}
    , dma_stream::periph_address {usart.transmit_data_adr()},
    , dma_stream::inc_memory
    , dma_stream::memory_size::byte8
    , dma_stream::periph_size::byte8
    , dma_stream::transfer_complete_interrupt::enable
);
```
Каждый парметр в методе set - отдельный тип, по которому можно понять, в какой регистр надо записать значение, а значит во время компиляции можно оптимизировать обращение к регистрам. Метод вариодический, поэтому аргументов может быть любое количество, но при этом должна присутсвовать проверка, что все аргументы относятся к данной переферии и аргументы не повторяются. Таким образом, всё можно будет соптимизироватьно и проверить на этапе компиляции.
