open Migrate_parsetree;
open Ast_408;

let name = "lenses";

let attributesContainName = attributes =>
  switch (
    attributes
    |> List.filter(
         fun
         | {Parsetree.attr_name: {txt}} when txt == name => true
         | {
             Parsetree.attr_name: {txt: "deriving"},
             attr_payload:
               PStr([
                 {
                   pstr_desc:
                     Pstr_eval(
                       {pexp_desc: Pexp_ident({txt: Lident(txt)})},
                       _,
                     ),
                 },
               ]),
           }
             when txt == name =>
           true
         | _ => false,
       )
  ) {
  | [] => false
  | _ => true
  };

let mapStructureItem =
  Ast_mapper.(
    Parsetree.(
      Asttypes.(
        mapper =>
          fun
          | {
              pstr_desc:
                Pstr_type(
                  _,
                  [
                    {
                      ptype_attributes: attributes,
                      ptype_name: typeName,
                      ptype_kind: Ptype_record(labels),
                    },
                  ],
                ),
              pstr_loc: loc,
            } as _type
              when attributesContainName(attributes) => [
              _type,
              ...labels
                 |> List.map(({pld_name: {txt: name}}) => {
                      let pname = Ast_helper.Pat.var(~loc, {txt: name, loc});

                      let ename = {txt: Longident.parse(name), loc};

                      let getter = [%expr
                        (
                          record => [%e
                            Ast_helper.Exp.field(
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
                             Ast_helper.Exp.record(
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
                        let [%p pname] =
                          Optic.Lens.make([%e getter], [%e setter])
                      ];
                    }),
            ]
          | structureItem => [mapper.structure_item(mapper, structureItem)]
      )
    )
  );

let mapper = (_config, _cookies) => {
  ...Ast_mapper.default_mapper,
  // TODO: Make the ppx work in mli/rei/resi context (signature)
  structure: (mapper, structure) =>
    structure |> List.map(mapStructureItem(mapper)) |> List.concat,
};

Migrate_parsetree.Driver.register(
  ~name,
  ~args=[],
  Migrate_parsetree.Versions.ocaml_408,
  mapper,
);

let argv =
  switch (Sys.argv) {
  | [|program, input_file, output_file|] => [|
      program,
      input_file,
      "-o",
      output_file,
      "--dump-ast",
    |]
  | _ => Sys.argv
  };

Migrate_parsetree.Driver.run_main(~argv, ());
