.text
# cette directive sert a rendre l'etiquette publique
    .globl handler_timer
# debut du traitant
handler_timer:
# sauvegarde des registres importants
    pusha
# appel a la fonction C realisant le traitant
    call handler_timer_C
# restauration des registres importants
    popa
# fin du traitant
    iret