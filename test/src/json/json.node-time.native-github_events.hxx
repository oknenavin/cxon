// Copyright (c) 2017-2025 oknenavin.
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

    struct actor_type {
        std::string gravatar_id;
        std::string login;
        std::string avatar_url;
        std::string url;
        unsigned long long id = 0;

        CXON_JSON_CLS_MEMBER(actor_type,
            CXON_JSON_CLS_FIELD_ASIS(gravatar_id),
            CXON_JSON_CLS_FIELD_ASIS(login),
            CXON_JSON_CLS_FIELD_ASIS(avatar_url),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(id)
        )
    };

    struct repo_type {
        std::string url;
        unsigned long long id = 0;
        std::string name;

        CXON_JSON_CLS_MEMBER(repo_type, CXON_JSON_CLS_FIELD_ASIS(url), CXON_JSON_CLS_FIELD_ASIS(id), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct author_type {
        std::string email;
        std::string name;

        CXON_JSON_CLS_MEMBER(author_type, CXON_JSON_CLS_FIELD_ASIS(email), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct commit_type {
        std::string url;
        std::string message;
        bool distinct = true;
        std::string sha;
        author_type author;

        CXON_JSON_CLS_MEMBER(commit_type,
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(message),
            CXON_JSON_CLS_FIELD_ASIS(distinct),
            CXON_JSON_CLS_FIELD_ASIS(sha),
            CXON_JSON_CLS_FIELD_ASIS(author)
        )
    };

    struct user_type {
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
        unsigned long long id = 0;
        std::string starred_url;
        std::string events_url;
        std::string followers_url;
        std::string following_url;

        bool operator ==(const user_type& o) const {
            return  url == o.url && gists_url == o.gists_url && gravatar_id == o.gravatar_id &&
                    type == o.type && avatar_url == o.avatar_url && subscriptions_url == o.subscriptions_url &&
                    organizations_url == o.organizations_url && received_events_url == o.received_events_url &&
                    repos_url == o.repos_url && login == o.login && id == o.id && starred_url == o.starred_url &&
                    events_url == o.events_url && followers_url == o.followers_url && following_url == o.following_url
            ;
        }

        CXON_JSON_CLS_MEMBER(user_type,
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

    struct forkee_type {
        std::string description;
        bool fork = false;
        std::string url;
        std::string language;
        std::string stargazers_url;
        std::string clone_url;
        std::string tags_url;
        std::string full_name;
        std::string merges_url;
        unsigned forks = 0;
        bool private_ = false;
        std::string git_refs_url;
        std::string archive_url;
        std::string collaborators_url;
        user_type owner;
        std::string languages_url;
        std::string trees_url;
        std::string labels_url;
        std::string html_url;
        std::string pushed_at;
        std::string created_at;
        bool has_issues = false;
        std::string forks_url;
        std::string branches_url;
        std::string commits_url;
        std::string notifications_url;
        unsigned open_issues = 0;
        std::string contents_url;
        std::string blobs_url;
        std::string issues_url;
        std::string compare_url;
        std::string issue_events_url;
        std::string name;
        std::string updated_at;
        std::string statuses_url;
        unsigned forks_count = 0;
        std::string assignees_url;
        std::string ssh_url;
        bool public_;
        bool has_wiki;
        std::string subscribers_url;
        std::optional<std::string> mirror_url;
        unsigned watchers_count;
        unsigned long long id = 0;
        bool has_downloads = false;
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
        unsigned open_issues_count = 0;
        std::string keys_url;
        std::string git_url;
        std::string contributors_url;
        unsigned size = 0;
        unsigned watchers = 0;

        CXON_JSON_CLS_MEMBER(forkee_type,
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

    struct pull_request_type {
        std::optional<std::string> html_url;
        std::optional<std::string> patch_url;
        std::optional<std::string> diff_url;

        CXON_JSON_CLS_MEMBER(pull_request_type, CXON_JSON_CLS_FIELD_ASIS(html_url), CXON_JSON_CLS_FIELD_ASIS(patch_url), CXON_JSON_CLS_FIELD_ASIS(diff_url))
    };

    struct issue_type {
        user_type user;
        std::string url;
        std::vector<std::string> labels;
        std::string html_url;
        std::string labels_url;
        pull_request_type pull_request;
        std::string created_at;
        std::optional<std::string> closed_at;
        std::optional<std::string> milestone;
        std::string title;
        std::string body;
        std::string updated_at;
        unsigned number = 0;
        std::string state;
        std::optional<user_type> assignee;
        unsigned long long id = 0;
        std::string events_url;
        std::string comments_url;
        unsigned comments = 0;

        CXON_JSON_CLS_MEMBER(issue_type,
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

    struct comment_type {
        user_type user;
        std::string url;
        std::string issue_url;
        std::string created_at;
        std::string body;
        std::string updated_at;
        unsigned long long id = 0;

        bool operator ==(const comment_type& o) const {
            return  user == o.user && url == o.url && issue_url == o.issue_url &&
                    created_at == o.created_at && body == o.body &&
                    updated_at == o.updated_at && id == o.id
            ;
        }

        CXON_JSON_CLS_MEMBER(comment_type,
            CXON_JSON_CLS_FIELD_ASIS(user),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(issue_url),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(body),
            CXON_JSON_CLS_FIELD_ASIS(updated_at),
            CXON_JSON_CLS_FIELD_ASIS(id)
        )
    };

    struct page_type {
        std::string page_name;
        std::string html_url;
        std::string title;
        std::string sha;
        std::optional<std::string> summary;
        std::string action;

        CXON_JSON_CLS_MEMBER(page_type,
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
        std::vector<commit_type> commits;
        unsigned distinct_size = 0;
        std::optional<std::string> ref;
        unsigned long long push_id = 0;
        std::string before;
        std::string head;
        unsigned size = 0;

        payload_push_event(
            std::vector<commit_type>&& commits,
            unsigned distinct_size,
            std::optional<std::string>&& ref,
            unsigned long long push_id,
            std::string&& before,
            std::string&& head,
            unsigned size
        ) : payload(payload::push_event),
            commits(std::move(commits)),
            distinct_size(distinct_size),
            ref(std::move(ref)),
            push_id(push_id),
            before(std::move(before)),
            head(std::move(head)),
            size(size)
        {
        }

        CXON_JSON_CLS_WRITE_MEMBER(payload_push_event,
            CXON_JSON_CLS_FIELD_ASIS(commits),
            CXON_JSON_CLS_FIELD_ASIS(distinct_size),
            CXON_JSON_CLS_FIELD_ASIS(ref),
            CXON_JSON_CLS_FIELD_ASIS(push_id),
            CXON_JSON_CLS_FIELD_ASIS(before),
            CXON_JSON_CLS_FIELD_ASIS(head),
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
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(master_branch),
            CXON_JSON_CLS_FIELD_ASIS(ref),
            CXON_JSON_CLS_FIELD_ASIS(ref_type)
        )
    };

    struct payload_fork_event : payload {
        forkee_type forkee;

        payload_fork_event(forkee_type&& forkee) : payload(payload::fork_event), forkee(std::move(forkee)) {}

        CXON_JSON_CLS_WRITE_MEMBER(payload_fork_event, CXON_JSON_CLS_FIELD_ASIS(forkee))
    };

    struct payload_watch_event : payload {
        std::string action;

        payload_watch_event(std::string&& action) : payload(payload::watch_event), action(std::move(action)) {}

        CXON_JSON_CLS_WRITE_MEMBER(payload_watch_event, CXON_JSON_CLS_FIELD_ASIS(action))
    };

    struct payload_issue_comment_event : payload {
        std::string action;
        issue_type issue;
        comment_type comment;

        payload_issue_comment_event(std::string&& action, issue_type&& issue, comment_type&& comment)
        :   payload(payload::issue_comment_event), action(std::move(action)), issue(std::move(issue)), comment(std::move(comment))
        {
        }

        CXON_JSON_CLS_WRITE_MEMBER(payload_issue_comment_event,
            CXON_JSON_CLS_FIELD_ASIS(action),
            CXON_JSON_CLS_FIELD_ASIS(issue),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(comment, self.comment == comment_type {})
        )
    };

    struct payload_gollum_event : payload {
        std::vector<page_type> pages;

        payload_gollum_event(std::vector<page_type>&& pages) : payload(payload::gollum_event), pages(std::move(pages)) {}

        CXON_JSON_CLS_WRITE_MEMBER(payload_gollum_event, CXON_JSON_CLS_FIELD_ASIS(pages))
    };

    struct payload_proxy {
        std::vector<commit_type> commits;
        unsigned distinct_size = 0;
        std::optional<std::string> ref;
        unsigned long long push_id = -1;
        std::string head;
        std::string before;
        unsigned size = 0;
        std::string description;
        std::string master_branch;
        std::string ref_type;
        forkee_type forkee;
        std::string action;
        issue_type issue;
        comment_type comment;
        std::vector<page_type> pages;

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
            CXON_JSON_CLS_FIELD_ASIS_DFLT(comment, self.comment == comment_type {}),
            CXON_JSON_CLS_FIELD_ASIS(pages)
        )
    };

    struct org_type {
        std::string gravatar_id;
        std::string login;
        std::string avatar_url;
        std::string url;
        unsigned long long id = 0;

        CXON_JSON_CLS_MEMBER(org_type,
            CXON_JSON_CLS_FIELD_ASIS(gravatar_id),
            CXON_JSON_CLS_FIELD_ASIS(login),
            CXON_JSON_CLS_FIELD_ASIS(avatar_url),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(id)
        )
    };

    struct event_type {
        std::string type;
        std::string created_at;
        actor_type actor;
        repo_type repo;
        bool public_ = false;
        org_type org;
        std::unique_ptr<struct payload> payload;
        std::string id;

        CXON_JSON_CLS_MEMBER(event_type,
            CXON_JSON_CLS_FIELD_ASIS(type),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(actor),
            CXON_JSON_CLS_FIELD_ASIS(repo),
            CXON_JSON_CLS_FIELD_NAME("public", public_),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(org, self.org.gravatar_id == "" && self.org.login == "" && self.org.avatar_url == "" && self.org.url == "" && self.org.id == 0),
            CXON_JSON_CLS_FIELD_ASIS(payload),
            CXON_JSON_CLS_FIELD_ASIS(id)
        )
    };

    using object = std::vector<event_type>;

}}  // test::github_events

namespace cxon {

    template <typename X>
        struct read<JSON<X>, test::github_events::payload*> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(test::github_events::payload*& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx))
                        return false;
                    if (cio::peek(i, e) == *Y::id::nil)
                        return read_value<Y>(t, i, e, cx);
                    using namespace test::github_events;
                    payload_proxy p;
                        if (read_value<Y>(p, i, e, cx)) {
                            if (p.push_id != (unsigned long long)-1)
                                return (t = alc::make_in_context<payload_push_event>(
                                    cx, std::move(p.commits), p.distinct_size, std::move(p.ref), p.push_id, std::move(p.before), std::move(p.head), p.size
                                ));
                            if (!p.ref_type.empty())
                                return (t = alc::make_in_context<payload_create_event>(
                                    cx, std::move(p.ref), std::move(p.ref_type), std::move(p.description), std::move(p.master_branch)
                                ));
                            if (!p.forkee.url.empty())
                                return (t = alc::make_in_context<payload_fork_event>(
                                    cx, std::move(p.forkee)
                                ));
                            if (!p.issue.url.empty())
                                return (t = alc::make_in_context<payload_issue_comment_event>(
                                    cx, std::move(p.action), std::move(p.issue), std::move(p.comment)
                                ));
                            if (!p.action.empty())
                                return (t = alc::make_in_context<payload_watch_event>(
                                    cx, std::move(p.action)
                                ));
                            if (!p.pages.empty())
                                return (t = alc::make_in_context<payload_gollum_event>(
                                    cx, std::move(p.pages)
                                ));
                        }
                    return false;
                }
        };

    template <typename X>
        struct write<JSON<X>, test::github_events::payload*> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const test::github_events::payload* t, Cx& cx) {
                    if (!t)
                        return write_value<Y>(o, nullptr, cx);
                    using namespace test::github_events;
                    switch (t->type) {
                        case payload::push_event:
                            return write_value<Y>(o, static_cast<const payload_push_event&>(*t), cx);
                        case payload::create_event:
                            return write_value<Y>(o, static_cast<const payload_create_event&>(*t), cx);
                        case payload::fork_event:
                            return write_value<Y>(o, static_cast<const payload_fork_event&>(*t), cx);
                        case payload::issue_comment_event:
                            return write_value<Y>(o, static_cast<const payload_issue_comment_event&>(*t), cx);
                        case payload::watch_event:
                            return write_value<Y>(o, static_cast<const payload_watch_event&>(*t), cx);
                        case payload::gollum_event:
                            return write_value<Y>(o, static_cast<const payload_gollum_event&>(*t), cx);
                    }
                    return false;
                }
        };

}

#endif // CXON_HAS_LIB_STD_OPTIONAL

#endif // CXON_JSON_TEST_NATIVE_GITHUB_EVENTS_HXX_
