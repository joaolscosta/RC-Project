# RC-Project

## TODO:

### UDP

- [ ] list_user
- [ ] show_record

### TCP

- [ ] open
- [ ] close
- [ ] show_asset
- [ ] bid

### LOCAL

- [ ] exit

## Ideias:

- As funções do actions estão todas super semelhantes faz questionar o pq de ter tanta função diferente que faz o msm. Talvez tornar mais modular
  (Men então não é que eu fiz as funções de verificação e depois estava a verificar as verificações AHAHAHAH já meti aquilo como queria thanks)

-

## Dúvidas:

- No unregister temos que verificar se o cliente está logado no cliente ou no servidor?

- (Recente) logout por parte do SV ou de USER logout NOK. Que acontece se fizer logo logout primeira coisa no programa?

- Q caralhos acontece se o user connectar se a orts ou ips errados etc..

# Stresses

- Cada vez que dou login a um utilizador, se quiser dar login a outro não aparece

- RECENTE Cuidado com o timeactive n tenho acerteza em q contexto se deve usar/se é cad avez que dou display ao status do auction e se der display dou update tmb ao aucition acrescentando o end file caso tenha excedido o prazo?

- PRECISO DE VERIFICAR NO BID SE O AUCTION EXISTE EM PRIMEIRO LUGAR ELES N PEDEM PA CHECKAR ND DISSO IDK

- TMB N DIZEM TIMEOUT DAS SOCKETS ASSUMO Q É OU N PRECISO?

- NA BID TENHO DE VERIFICAR SE A BID QUE SE PODE FZR É SMP MAIOR Q A ULTIMA FEITA, MAS CASO SEJA A PRIMEIRA BID A FZR PRECISO DE CHECKAR SE É MAIOR QUE O START_VALUE PEDIDO PELO AUCTION? SE ISSO ACONTECER TENHO DE MANDAR A MSM MSG DE ERRO?

- QUANDO É Q CHECKO SE O AUCTION TA ATIVO OU N OU SE O TEMPO ESTIPULADO (TIME_ACTIVE) TEM DE SER CADA VEZ QUE FACA UMA INTERACAO OCM AUCTIONS TIPO NO MA OU ASSIM? 