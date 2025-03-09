# modbus_bridge

Repositório encerrado.

## Como funciona
Na ponte, são instanciados um *server* TCP e um *client* RTU. O servidor escuta a porta 502 e é acessível pela rede à qual a placa é conectada quando ligada. As credenciais de acesso à rede são gerenciadas pelo *wifi manager*. O *client* assume a porta serial definida pela *flag* **SW_AS_RT**. A serial complementar é usada para depuração. Inicializada, a ponte transmite todo pacote modbus recebido em seu *server* para o *client* e transmite a resposta do *client* para o *server*. A página web permite configurar e verificar paridade e taxa de bits da interface RTU.

- inicialização:
  - parametros das interfaces de debug e RTU são buscadas na memória flash, usando a biblioteca `Preferences`. Se não encontradas, são definidas com valores padrão.
  - a página web é inicializada conforme a seleção da interface RTU.
  - a ponte é conectada à rede pelo *wifi manager* ou por credenciais estáticas, conforme definido na compilação.
  - um servidor assincrono é inicializado com a página de configuração.
  - o *server* TCP é inicializado na porta 502.
  - o *client* RTU é inicializado na porta serial definida pela *flag* **SW_AS_RTU**.
  - as *callback* são definidas para o *server* e o *client*.

- execução:
  - mensagens recebidas pelo *server* são transmitidas ao *client*.
  - respostas do *client* são transmitidas ao *server*.
  - mensagens de erro são transmitidas à interface de depuração.
  - paridade e taxa de bits da interface RTU podem ser configuradas pela página web.

## Como usar
### Configuração estática
- **USE_WIFI_MANAGER** (`define`, [`true`, `false`]): *wifi manager* será usado para configurar a rede?
- **SW_AS_RTU** (`define`, [`true`, `false`]): *client* assumirá a serial em software? Se falso, a serial é assumida em hardware. A interface de depuração assume a serial complementar.
- **RX, TX** (`const`, [`pin`]): pinos da serial em software.
- **serial defaults** (argumentos para `Configs<RTUSerial, DBGSerial>()`): taxa de bits e paridade das interfaces RTU e de depuração.

### Configuração dinâmica
- **\<IP DA PONTE\>/configure** (`GET`): retorna a página de configuração. <br>
Todas as mudanças são aplicadas imediatamente, sem necessidade de reinicialização.

## TODO
- [ ] Repassar mensagens de erro do *server* para o *client* <br>
Atualmente, as mensagens de erro são consumidas pela ponte e transmitidas pela interface de depuração. Esse comportamento só é adequado para a mensagem de timeout. As demais podem ser exibidas na depuração, mas devem, também, ser repassadas ao *client*.
- [ ] Extender a funcionalidades a multiplas transações concomitantes. <br>
Não deve ser difícil. Não é possível, no estado atual, porque os identificadores são armazenados em variáveis únicas. Um map deve resolver.
- [ ] Habilitar seleção entre RS232 e RS485. <br>
Atualmente, só é possível usar RS232. A seleção pode ser em tempo de compilação.

## Sugestões e comentários
- Manter a interface de depuração na mesma serial em que o carregamento do código é feito é interessante porque permite manter todo acesso de usuário em uma única interface. Assim, fica simples de desacoplar a ponte quando em funcionamento. Entretanto, como o modbus é crítico em tempo, a interferência da interface em hardware sobre a serial em software prejudica as transmissões. Quando estiver funcionando bem, as mensagens de depuração podem ser reduzidas até que possam ser transmitidas pela interface de hardware sem interferir nas transmissões em software.

- `SoftwareSerial`e `HardwareSerial` são classes distintas e não implicitamente convertíveis. Por isso, para permitir a seleção entre as duas, *templates* são usados. Mas os `enums` de paridade são distintos entre as classes e, também, não implicitamente convertíveis. Daí, toda uma complexa gestão de tipos foi implementada. Repensando, essa abordagem, apesar de ser interessante e evidenciar recursos da linguagem, não é prática, principalmente para manutenção. Quem seguir com o projeto deve considerar a solução na *branch* **hacky_serial**, na qual simples `#define` resolvem o problema.

- Uma ESP32, com três seriais em hardware, seria mais prática e, dependendo de onde e quando comprada, tem preço similar. Ainda assim, é válido o exercício de usar a ESP8266.
