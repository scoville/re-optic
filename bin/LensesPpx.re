open Ppxlib;

let name = "lenses";

let str_type_decl =
  Deriving.Generator.make_noarg((~loc, ~path as _, (_, type_declaration)) => {
    switch (type_declaration) {
    | [
        {
          ptype_name: {txt: typeName},
          ptype_kind: Ptype_record(labels),
          ptype_loc: loc,
        },
      ] =>
      let lenses =
        labels
        |> List.map(({pld_name: {txt: name}}) => {
             let pname = Ast_builder.Default.pvar(~loc, name);

             let ename = Loc.make(Longident.Lident(name), ~loc);

             let getter = [%expr
               (
                 record => [%e
                   Ast_builder.Default.pexp_field(
                     ~loc,
                     [%expr record],
                     [%expr [%e ename]],
                   )
                 ]
               )
             ];

             let setter = [%expr
               ([@ocaml.warning "-23"]
                (
                  (newValue, record) => [%e
                    Ast_builder.Default.pexp_record(
                      ~loc,
                      [
                        (
                          {loc, txt: Longident.Lident(name)},
                          [%expr newValue],
                        ),
                      ],
                      Some([%expr record]),
                    )
                  ]
                ))
             ];

             [%stri
               [@ocaml.warning "-32"]
               let [%p pname] = Optic.Lens.make([%e getter], [%e setter])
             ];
           });

      lenses;
    | _ => raise(Failure("You must provide only one record"))
    }
  });

// TODO: Make the ppx work in mli/rei context
let sig_type_decl =
  Deriving.Generator.make_noarg((~loc as _, ~path as _, _) => []);

Deriving.add(name, ~str_type_decl, ~sig_type_decl);
