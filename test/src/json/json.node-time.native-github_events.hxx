// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_GITHUB_EVENTS_HXX_
#define CXON_JSON_TEST_NATIVE_GITHUB_EVENTS_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/memory.hxx"


#ifdef CXON_HAS_LIB_STD_OPTIONAL

namespace test { namespace github_events {

    struct actor {
        std::string gravatar_id;
        std::string login;
        std::string avatar_url;
        std::string url;
        unsigned long long id;

        CXON_JSON_CLS_MEMBER(actor,
            CXON_JSON_CLS_FIELD_ASIS(gravatar_id),
            CXON_JSON_CLS_FIELD_ASIS(login),
            CXON_JSON_CLS_FIELD_ASIS(avatar_url),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(id)
        )
    };

    struct repo {
        std::string url;
        unsigned long long id;
        std::string name;

        CXON_JSON_CLS_MEMBER(repo, CXON_JSON_CLS_FIELD_ASIS(url), CXON_JSON_CLS_FIELD_ASIS(id), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct author {
        std::string email;
        std::string name;

        CXON_JSON_CLS_MEMBER(author, CXON_JSON_CLS_FIELD_ASIS(email), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct commit {
        std::string url;
        std::string message;
        bool distinct;
        std::string sha;
        struct author author;

        CXON_JSON_CLS_MEMBER(commit,
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(message),
            CXON_JSON_CLS_FIELD_ASIS(distinct),
            CXON_JSON_CLS_FIELD_ASIS(sha),
            CXON_JSON_CLS_FIELD_ASIS(author)
        )
    };

    struct user {
        std::string url;
        std::string gists_url;
        std::string gravatar_id;
        std::string type;
        std::string avatar_url;
        std::string subscriptions_url;
        std::string organizations_url;
        std::string received_events_url;
        std::string repos_url;
        std::string login;
        unsigned long long id;
        std::string starred_url;
        std::string events_url;
        std::string followers_url;
        std::string following_url;

        CXON_JSON_CLS_MEMBER(user,
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(gists_url),
            CXON_JSON_CLS_FIELD_ASIS(gravatar_id),
            CXON_JSON_CLS_FIELD_ASIS(type),
            CXON_JSON_CLS_FIELD_ASIS(avatar_url),
            CXON_JSON_CLS_FIELD_ASIS(subscriptions_url),
            CXON_JSON_CLS_FIELD_ASIS(organizations_url),
            CXON_JSON_CLS_FIELD_ASIS(received_events_url),
            CXON_JSON_CLS_FIELD_ASIS(repos_url),
            CXON_JSON_CLS_FIELD_ASIS(login),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(starred_url),
            CXON_JSON_CLS_FIELD_ASIS(events_url),
            CXON_JSON_CLS_FIELD_ASIS(followers_url),
            CXON_JSON_CLS_FIELD_ASIS(following_url)
        )
    };

    struct forkee {
        std::string description;
        bool fork;
        std::string url;
        std::string language;
        std::string stargazers_url;
        std::string clone_url;
        std::string tags_url;
        std::string full_name;
        std::string merges_url;
        unsigned forks;
        bool private_;
        std::string git_refs_url;
        std::string archive_url;
        std::string collaborators_url;
        struct user owner;
        std::string languages_url;
        std::string trees_url;
        std::string labels_url;
        std::string html_url;
        std::string pushed_at;
        std::string created_at;
        bool has_issues;
        std::string forks_url;
        std::string branches_url;
        std::string commits_url;
        std::string notifications_url;
        unsigned open_issues;
        std::string contents_url;
        std::string blobs_url;
        std::string issues_url;
        std::string compare_url;
        std::string issue_events_url;
        std::string name;
        std::string updated_at;
        std::string statuses_url;
        unsigned forks_count;
        std::string assignees_url;
        std::string ssh_url;
        bool public_;
        bool has_wiki;
        std::string subscribers_url;
        std::optional<std::string> mirror_url;
        unsigned watchers_count;
        unsigned long long id;
        bool has_downloads;
        std::string git_commits_url;
        std::string downloads_url;
        std::string pulls_url;
        std::optional<std::string> homepage;
        std::string issue_comment_url;
        std::string hooks_url;
        std::string subscription_url;
        std::string milestones_url;
        std::string svn_url;
        std::string events_url;
        std::string git_tags_url;
        std::string teams_url;
        std::string comments_url;
        unsigned open_issues_count;
        std::string keys_url;
        std::string git_url;
        std::string contributors_url;
        unsigned size = 0;
        unsigned watchers;

        CXON_JSON_CLS_MEMBER(forkee,
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(fork),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(language),
            CXON_JSON_CLS_FIELD_ASIS(stargazers_url),
            CXON_JSON_CLS_FIELD_ASIS(clone_url),
            CXON_JSON_CLS_FIELD_ASIS(tags_url),
            CXON_JSON_CLS_FIELD_ASIS(full_name),
            CXON_JSON_CLS_FIELD_ASIS(merges_url),
            CXON_JSON_CLS_FIELD_ASIS(forks),
            CXON_JSON_CLS_FIELD_NAME("private", private_),
            CXON_JSON_CLS_FIELD_ASIS(git_refs_url),
            CXON_JSON_CLS_FIELD_ASIS(archive_url),
            CXON_JSON_CLS_FIELD_ASIS(collaborators_url),
            CXON_JSON_CLS_FIELD_ASIS(owner),
            CXON_JSON_CLS_FIELD_ASIS(languages_url),
            CXON_JSON_CLS_FIELD_ASIS(trees_url),
            CXON_JSON_CLS_FIELD_ASIS(labels_url),
            CXON_JSON_CLS_FIELD_ASIS(html_url),
            CXON_JSON_CLS_FIELD_ASIS(pushed_at),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(has_issues),
            CXON_JSON_CLS_FIELD_ASIS(forks_url),
            CXON_JSON_CLS_FIELD_ASIS(branches_url),
            CXON_JSON_CLS_FIELD_ASIS(commits_url),
            CXON_JSON_CLS_FIELD_ASIS(notifications_url),
            CXON_JSON_CLS_FIELD_ASIS(open_issues),
            CXON_JSON_CLS_FIELD_ASIS(contents_url),
            CXON_JSON_CLS_FIELD_ASIS(blobs_url),
            CXON_JSON_CLS_FIELD_ASIS(issues_url),
            CXON_JSON_CLS_FIELD_ASIS(compare_url),
            CXON_JSON_CLS_FIELD_ASIS(issue_events_url),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(updated_at),
            CXON_JSON_CLS_FIELD_ASIS(statuses_url),
            CXON_JSON_CLS_FIELD_ASIS(forks_count),
            CXON_JSON_CLS_FIELD_ASIS(assignees_url),
            CXON_JSON_CLS_FIELD_ASIS(ssh_url),
            CXON_JSON_CLS_FIELD_NAME("public", public_),
            CXON_JSON_CLS_FIELD_ASIS(has_wiki),
            CXON_JSON_CLS_FIELD_ASIS(subscribers_url),
            CXON_JSON_CLS_FIELD_ASIS(mirror_url),
            CXON_JSON_CLS_FIELD_ASIS(watchers_count),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(has_downloads),
            CXON_JSON_CLS_FIELD_ASIS(git_commits_url),
            CXON_JSON_CLS_FIELD_ASIS(downloads_url),
            CXON_JSON_CLS_FIELD_ASIS(pulls_url),
            CXON_JSON_CLS_FIELD_ASIS(homepage),
            CXON_JSON_CLS_FIELD_ASIS(issue_comment_url),
            CXON_JSON_CLS_FIELD_ASIS(hooks_url),
            CXON_JSON_CLS_FIELD_ASIS(subscription_url),
            CXON_JSON_CLS_FIELD_ASIS(milestones_url),
            CXON_JSON_CLS_FIELD_ASIS(svn_url),
            CXON_JSON_CLS_FIELD_ASIS(events_url),
            CXON_JSON_CLS_FIELD_ASIS(git_tags_url),
            CXON_JSON_CLS_FIELD_ASIS(teams_url),
            CXON_JSON_CLS_FIELD_ASIS(comments_url),
            CXON_JSON_CLS_FIELD_ASIS(open_issues_count),
            CXON_JSON_CLS_FIELD_ASIS(keys_url),
            CXON_JSON_CLS_FIELD_ASIS(git_url),
            CXON_JSON_CLS_FIELD_ASIS(contributors_url),
            CXON_JSON_CLS_FIELD_ASIS(size),
            CXON_JSON_CLS_FIELD_ASIS(watchers)
        )
    };

    struct pull_request {
        std::optional<std::string> html_url;
        std::optional<std::string> patch_url;
        std::optional<std::string> diff_url;

        CXON_JSON_CLS_MEMBER(pull_request, CXON_JSON_CLS_FIELD_ASIS(html_url), CXON_JSON_CLS_FIELD_ASIS(patch_url), CXON_JSON_CLS_FIELD_ASIS(diff_url))
    };

    struct issue {
        struct user user;
        std::string url;
        std::vector<std::string> labels;
        std::string html_url;
        std::string labels_url;
        struct pull_request pull_request;
        std::string created_at;
        std::optional<std::string> closed_at;
        std::optional<std::string> milestone;
        std::string title;
        std::string body;
        std::string updated_at;
        unsigned number;
        std::string state;
        std::optional<struct user> assignee;
        unsigned long long id;
        std::string events_url;
        std::string comments_url;
        unsigned comments;

        CXON_JSON_CLS_MEMBER(issue,
            CXON_JSON_CLS_FIELD_ASIS(user),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(labels),
            CXON_JSON_CLS_FIELD_ASIS(html_url),
            CXON_JSON_CLS_FIELD_ASIS(labels_url),
            CXON_JSON_CLS_FIELD_ASIS(pull_request),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(closed_at),
            CXON_JSON_CLS_FIELD_ASIS(milestone),
            CXON_JSON_CLS_FIELD_ASIS(title),
            CXON_JSON_CLS_FIELD_ASIS(body),
            CXON_JSON_CLS_FIELD_ASIS(updated_at),
            CXON_JSON_CLS_FIELD_ASIS(number),
            CXON_JSON_CLS_FIELD_ASIS(state),
            CXON_JSON_CLS_FIELD_ASIS(assignee),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(events_url),
            CXON_JSON_CLS_FIELD_ASIS(comments_url),
            CXON_JSON_CLS_FIELD_ASIS(comments)
        )
    };

    struct comment {
        struct user user;
        std::string url;
        std::string issue_url;
        std::string created_at;
        std::string body;
        std::string updated_at;
        unsigned long long id;

        CXON_JSON_CLS_MEMBER(comment,
            CXON_JSON_CLS_FIELD_ASIS(user),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(issue_url),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(body),
            CXON_JSON_CLS_FIELD_ASIS(updated_at),
            CXON_JSON_CLS_FIELD_ASIS(id)
        )
    };

    struct page {
        std::string page_name;
        std::string html_url;
        std::string title;
        std::string sha;
        std::optional<std::string> summary;
        std::string action;

        CXON_JSON_CLS_MEMBER(page,
            CXON_JSON_CLS_FIELD_ASIS(page_name),
            CXON_JSON_CLS_FIELD_ASIS(html_url),
            CXON_JSON_CLS_FIELD_ASIS(title),
            CXON_JSON_CLS_FIELD_ASIS(sha),
            CXON_JSON_CLS_FIELD_ASIS(summary),
            CXON_JSON_CLS_FIELD_ASIS(action)
        )
    };

    struct payload {
        enum type{ push_event, create_event, fork_event, watch_event, issue_comment_event, gollum_event } type;

        payload(enum type t) : type(t) {}
        virtual ~payload() {}
    };

    struct payload_push_event : payload {
        std::vector<commit> commits;
        unsigned distinct_size;
        std::optional<std::string> ref;
        unsigned long long push_id;
        std::string head;
        std::string before;
        unsigned size;

        payload_push_event(
            std::vector<commit>&& commits,
            unsigned distinct_size,
            std::optional<std::string>&& ref,
            unsigned long long push_id,
            std::string&& head,
            std::string&& before,
            unsigned size
        ) : payload(payload::push_event),
            commits(std::move(commits)),
            distinct_size(distinct_size),
            ref(std::move(ref)),
            push_id(push_id),
            head(std::move(head)),
            before(std::move(before)),
            size(size)
        {
        }

        CXON_JSON_CLS_WRITE_MEMBER(payload_push_event,
            CXON_JSON_CLS_FIELD_ASIS(commits),
            CXON_JSON_CLS_FIELD_ASIS(distinct_size),
            CXON_JSON_CLS_FIELD_ASIS(ref),
            CXON_JSON_CLS_FIELD_ASIS(push_id),
            CXON_JSON_CLS_FIELD_ASIS(head),
            CXON_JSON_CLS_FIELD_ASIS(before),
            CXON_JSON_CLS_FIELD_ASIS(size)
        )
    };

    struct payload_create_event : payload {
        std::optional<std::string> ref;
        std::string ref_type;
        std::string description;
        std::string master_branch;

        payload_create_event(std::optional<std::string>&& ref, std::string&& ref_type, std::string&& description, std::string&& master_branch)
        :   payload(payload::create_event), ref(std::move(ref)), ref_type(std::move(ref_type)), description(std::move(description)), master_branch(std::move(master_branch))
        {
        }

        CXON_JSON_CLS_WRITE_MEMBER(payload_create_event,
            CXON_JSON_CLS_FIELD_ASIS(ref),
            CXON_JSON_CLS_FIELD_ASIS(ref_type),
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(master_branch)
        )
    };

    struct payload_fork_event : payload {
        struct forkee forkee;

        payload_fork_event(struct forkee&& forkee) : payload(payload::fork_event), forkee(std::move(forkee)) {}

        CXON_JSON_CLS_WRITE_MEMBER(payload_fork_event, CXON_JSON_CLS_FIELD_ASIS(forkee))
    };

    struct payload_watch_event : payload {
        std::string action;

        payload_watch_event(std::string&& action) : payload(payload::watch_event), action(std::move(action)) {}

        CXON_JSON_CLS_WRITE_MEMBER(payload_watch_event, CXON_JSON_CLS_FIELD_ASIS(action))
    };

    struct payload_issue_comment_event : payload {
        std::string action;
        struct issue issue;
        struct comment comment;

        payload_issue_comment_event(std::string&& action, struct issue&& issue, struct comment&& comment)
        :   payload(payload::issue_comment_event), action(std::move(action)), issue(std::move(issue)), comment(std::move(comment))
        {
        }

        CXON_JSON_CLS_WRITE_MEMBER(payload_issue_comment_event, CXON_JSON_CLS_FIELD_ASIS(action), CXON_JSON_CLS_FIELD_ASIS(issue), CXON_JSON_CLS_FIELD_ASIS(comment))
    };

    struct payload_gollum_event : payload {
        std::vector<page> pages;

        payload_gollum_event(std::vector<page>&& pages) : payload(payload::gollum_event), pages(std::move(pages)) {}

        CXON_JSON_CLS_WRITE_MEMBER(payload_gollum_event, CXON_JSON_CLS_FIELD_ASIS(pages))
    };

    struct payload_proxy {
        std::vector<commit> commits;
        unsigned distinct_size;
        std::optional<std::string> ref;
        unsigned long long push_id = -1;
        std::string head;
        std::string before;
        unsigned size = 0;
        std::string description;
        std::string master_branch;
        std::string ref_type;
        struct forkee forkee;
        std::string action;
        struct issue issue;
        struct comment comment;
        std::vector<page> pages;

        CXON_JSON_CLS_READ_MEMBER(payload_proxy,
            CXON_JSON_CLS_FIELD_ASIS(commits),
            CXON_JSON_CLS_FIELD_ASIS(distinct_size),
            CXON_JSON_CLS_FIELD_ASIS(ref),
            CXON_JSON_CLS_FIELD_ASIS(push_id),
            CXON_JSON_CLS_FIELD_ASIS(head),
            CXON_JSON_CLS_FIELD_ASIS(before),
            CXON_JSON_CLS_FIELD_ASIS(size),
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(master_branch),
            CXON_JSON_CLS_FIELD_ASIS(ref_type),
            CXON_JSON_CLS_FIELD_ASIS(forkee),
            CXON_JSON_CLS_FIELD_ASIS(action),
            CXON_JSON_CLS_FIELD_ASIS(issue),
            CXON_JSON_CLS_FIELD_ASIS(comment),
            CXON_JSON_CLS_FIELD_ASIS(pages)
        )
    };

    struct org {
        std::string gravatar_id;
        std::string login;
        std::string avatar_url;
        std::string url;
        unsigned long long id;

        CXON_JSON_CLS_MEMBER(org,
            CXON_JSON_CLS_FIELD_ASIS(gravatar_id),
            CXON_JSON_CLS_FIELD_ASIS(login),
            CXON_JSON_CLS_FIELD_ASIS(avatar_url),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(id)
        )
    };

    struct event {
        std::string type;
        std::string created_at;
        struct actor actor;
        struct repo repo;
        bool public_;
        std::unique_ptr<struct payload> payload;
        struct org org;
        std::string id;

        CXON_JSON_CLS_MEMBER(event,
            CXON_JSON_CLS_FIELD_ASIS(type),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(actor),
            CXON_JSON_CLS_FIELD_ASIS(repo),
            CXON_JSON_CLS_FIELD_NAME("public", public_),
            CXON_JSON_CLS_FIELD_ASIS(payload),
            CXON_JSON_CLS_FIELD_ASIS(org),
            CXON_JSON_CLS_FIELD_ASIS(id)
        )
    };

    using object = std::vector<event>;

}}  // test::github_events

namespace cxon {

    template <typename X>
        struct read<JSON<X>, test::github_events::payload*> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(test::github_events::payload*& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx))
                        return false;
                    if (cio::peek(i, e) == *Y::id::nil) { // TODO: needs buffering
                        II const o = i;
                        return  (cio::consume<Y>(Y::id::nil, i, e) && (t = nullptr, true)) ||
                                (cio::rewind(i, o), cx/json::read_error::unexpected)
                        ;
                    }
                    test::github_events::payload_proxy p;
                        if (read_value<Y>(p, i, e, cx)) {
                            if (p.push_id != (unsigned long long)-1)
                                return (t = new test::github_events::payload_push_event {
                                    std::move(p.commits), p.distinct_size, std::move(p.ref), p.push_id, std::move(p.head), std::move(p.before), p.size
                                });
                            if (!p.ref_type.empty())
                                return (t = new test::github_events::payload_create_event {
                                    std::move(p.ref), std::move(p.ref_type), std::move(p.description), std::move(p.master_branch)
                                });
                            if (!p.forkee.url.empty())
                                return (t = new test::github_events::payload_fork_event {
                                    std::move(p.forkee)
                                });
                            if (!p.issue.url.empty())
                                return (t = new test::github_events::payload_issue_comment_event {
                                    std::move(p.action), std::move(p.issue), std::move(p.comment)
                                });
                            if (!p.action.empty())
                                return (t = new test::github_events::payload_watch_event {
                                    std::move(p.action)
                                });
                            if (!p.pages.empty())
                                return (t = new test::github_events::payload_gollum_event {
                                    std::move(p.pages)
                                });
                        }
                        return false;
                }
        };

    template <typename X>
        struct write<JSON<X>, test::github_events::payload*> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const test::github_events::payload* t, Cx& cx) {
                    if (!t)
                        return cio::poke<Y>(o, Y::id::nil, cx);
                    switch (t->type) {
                        case test::github_events::payload::push_event:
                            return write_value<Y>(o, static_cast<const test::github_events::payload_push_event*>(t), cx);
                        case test::github_events::payload::create_event:
                            return write_value<Y>(o, static_cast<const test::github_events::payload_create_event*>(t), cx);
                        case test::github_events::payload::fork_event:
                            return write_value<Y>(o, static_cast<const test::github_events::payload_fork_event*>(t), cx);
                        case test::github_events::payload::issue_comment_event:
                            return write_value<Y>(o, static_cast<const test::github_events::payload_issue_comment_event*>(t), cx);
                        case test::github_events::payload::watch_event:
                            return write_value<Y>(o, static_cast<const test::github_events::payload_watch_event*>(t), cx);
                        case test::github_events::payload::gollum_event:
                            return write_value<Y>(o, static_cast<const test::github_events::payload_gollum_event*>(t), cx);
                    }
                    return false;
                }
        };

}

#endif // CXON_HAS_LIB_STD_OPTIONAL

#endif // CXON_JSON_TEST_NATIVE_GITHUB_EVENTS_HXX_
