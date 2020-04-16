module type OVERABLE = {
  type t('a, 'b);

  let over: (t('a, 'b), 'b => 'b, 'a) => 'a;
};

module Infix = (Overable: OVERABLE) => {
  let ( *~ ): (Overable.t('a, 'b), 'b => 'b, 'a) => 'a = Overable.over;
};
