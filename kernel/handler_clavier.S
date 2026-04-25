.text
# cette directive sert a rendre l'etiquette publique
    .globl handler_clavier
# debut du traitant
handler_clavier:
# sauvegarde des registres importants
    pusha
# appel a la fonction C realisant le traitant
    call handler_clavier_C
# restauration des registres importants
    popa
# fin du traitant
    iret