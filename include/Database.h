#pragma once

#include <sqlite_orm/sqlite_orm.h>
#include "Operator.h"
#include "Call.h"

inline auto createStorage(const std::string& filename)
{
    using namespace sqlite_orm;

    return make_storage(
        filename,

        make_table(
            "Operator",

            make_column("id",
                &Operator::id,
                primary_key().autoincrement()),

            make_column("name",
                &Operator::name,
                not_null()),

            make_column("surname",
                &Operator::surname,
                not_null()),

            make_column("department",
                &Operator::department,
                not_null()),

            make_column("experience",
                &Operator::experience)
        ),

        make_table(
            "Call",

            make_column("id",
                &Call::id,
                primary_key().autoincrement()),

            make_column("operator_id",
                &Call::operatorId),

            make_column("client_phone",
                &Call::clientPhone,
                not_null()),

            make_column("call_date",
                &Call::callDate,
                not_null()),

            make_column("duration",
                &Call::duration),

            make_column("result",
                &Call::result,
                not_null()),

            foreign_key(&Call::operatorId)
                .references(&Operator::id)
        )
    );
}